/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
/*
   Concepts and parts of this file have been contributed by Uladzimir Pylinsky
   aka barthess.
 */

/**
 * @file    STM32/i2c_lld.c
 * @brief   STM32 I2C subsystem low level driver source.
 *
 * @addtogroup I2C
 * @{
 */

#include "ch.h"
#include "hal.h"

#if HAL_USE_I2C || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define I2C1_RX_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_I2C_I2C1_RX_DMA_STREAM,                        \
                       STM32_I2C1_RX_DMA_CHN)

#define I2C1_TX_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_I2C_I2C1_TX_DMA_STREAM,                        \
                       STM32_I2C1_TX_DMA_CHN)

#define I2C2_RX_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_I2C_I2C2_RX_DMA_STREAM,                        \
                       STM32_I2C2_RX_DMA_CHN)

#define I2C2_TX_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_I2C_I2C2_TX_DMA_STREAM,                        \
                       STM32_I2C2_TX_DMA_CHN)

#define I2C3_RX_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_I2C_I2C3_RX_DMA_STREAM,                        \
                       STM32_I2C3_RX_DMA_CHN)

#define I2C3_TX_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_I2C_I2C3_TX_DMA_STREAM,                        \
                       STM32_I2C3_TX_DMA_CHN)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

#define I2C_EV5_MASTER_MODE_SELECT                                          \
  ((uint32_t)(((I2C_SR2_MSL | I2C_SR2_BUSY) << 16) | I2C_SR1_SB))
#define I2C_EV6_MASTER_TRA_MODE_SELECTED                                    \
  ((uint32_t)(((I2C_SR2_MSL|I2C_SR2_BUSY|I2C_SR2_TRA) << 16) |              \
              I2C_SR1_ADDR|I2C_SR1_TXE))
#define I2C_EV6_MASTER_REC_MODE_SELECTED                                    \
  ((uint32_t)(((I2C_SR2_MSL|I2C_SR2_BUSY)<< 16) | I2C_SR1_ADDR))
#define I2C_EV8_2_MASTER_BYTE_TRANSMITTED                                   \
  ((uint32_t)(((I2C_SR2_MSL | I2C_SR2_BUSY | I2C_SR2_TRA) << 16) |          \
              I2C_SR1_BTF | I2C_SR1_TXE))
#define I2C_EV_MASK 0x00FFFFFF

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief I2C1 driver identifier.*/
#if STM32_I2C_USE_I2C1 || defined(__DOXYGEN__)
I2CDriver I2CD1;
#endif

/** @brief I2C2 driver identifier.*/
#if STM32_I2C_USE_I2C2 || defined(__DOXYGEN__)
I2CDriver I2CD2;
#endif

/** @brief I2C3 driver identifier.*/
#if STM32_I2C_USE_I2C3 || defined(__DOXYGEN__)
I2CDriver I2CD3;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/* The following variables have debugging purpose only and are included when
   the option CH_DBG_ENABLE_ASSERTS is enabled.*/
#if CH_DBG_ENABLE_ASSERTS
static volatile uint16_t dbgSR1;
static volatile uint16_t dbgSR2;
static volatile uint16_t dbgCR1;
static volatile uint16_t dbgCR2;
#endif /* CH_DBG_ENABLE_ASSERTS */

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Wakes up the waiting thread.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] msg       wakeup message
 *
 * @notapi
 */
#define wakeup_isr(i2cp, msg) {                                             \
  chSysLockFromIsr();                                                       \
  if ((i2cp)->thread != NULL) {                                             \
    Thread *tp = (i2cp)->thread;                                            \
    (i2cp)->thread = NULL;                                                  \
    tp->p_u.rdymsg = (msg);                                                 \
    chSchReadyI(tp);                                                        \
  }                                                                         \
  chSysUnlockFromIsr();                                                     \
}

/**
 * @brief   Aborts an I2C transaction.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
static void i2c_lld_abort_operation(I2CDriver *i2cp) {
  I2C_TypeDef *dp = i2cp->i2c;

  /* Stops the I2C peripheral.*/
  dp->CR1 = I2C_CR1_SWRST;
  dp->CR1 = 0;
  dp->CR2 = 0;
  dp->SR1 = 0;

  /* Stops the associated DMA streams.*/
  dmaStreamDisable(i2cp->dmatx);
  dmaStreamDisable(i2cp->dmarx);
}

/**
 * @brief   Handling of stalled I2C transactions.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
static void i2c_lld_safety_timeout(void *p) {
  I2CDriver *i2cp = (I2CDriver *)p;

  if (i2cp->thread) {
    Thread *tp = i2cp->thread;
    i2c_lld_abort_operation(i2cp);
    i2cp->thread = NULL;
    tp->p_u.rdymsg = RDY_TIMEOUT;
    chSchReadyI(tp);
  }
}

/**
 * @brief   Set clock speed.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
static void i2c_lld_set_clock(I2CDriver *i2cp) {
  I2C_TypeDef *dp = i2cp->i2c;
  uint16_t regCCR, clock_div;
  int32_t clock_speed = i2cp->config->clock_speed;
  i2cdutycycle_t duty = i2cp->config->duty_cycle;

  chDbgCheck((i2cp != NULL) && (clock_speed > 0) && (clock_speed <= 4000000),
             "i2c_lld_set_clock");

  /* CR2 Configuration.*/
  dp->CR2 &= (uint16_t)~I2C_CR2_FREQ;
  dp->CR2 |= (uint16_t)I2C_CLK_FREQ;

  /* CCR Configuration.*/
  regCCR = 0;
  clock_div = I2C_CCR_CCR;

  if (clock_speed <= 100000) {
    /* Configure clock_div in standard mode.*/
    chDbgAssert(duty == STD_DUTY_CYCLE,
                "i2c_lld_set_clock(), #1",
                "Invalid standard mode duty cycle");

    /* Standard mode clock_div calculate: Tlow/Thigh = 1/1.*/
    chDbgAssert((STM32_PCLK1 % (clock_speed * 2)) == 0,
                "i2c_lld_set_clock(), #2",
                "PCLK1 must be divided without remainder");
    clock_div = (uint16_t)(STM32_PCLK1 / (clock_speed * 2));

    chDbgAssert(clock_div >= 0x04,
                "i2c_lld_set_clock(), #3",
                "Clock divider less then 0x04 not allowed");
    regCCR |= (clock_div & I2C_CCR_CCR);

    /* Sets the Maximum Rise Time for standard mode.*/
    dp->TRISE = I2C_CLK_FREQ + 1;
  }
  else if (clock_speed <= 400000) {
    /* Configure clock_div in fast mode.*/
    chDbgAssert((duty == FAST_DUTY_CYCLE_2) || (duty == FAST_DUTY_CYCLE_16_9),
                "i2c_lld_set_clock(), #4",
                "Invalid fast mode duty cycle");

    if (duty == FAST_DUTY_CYCLE_2) {
      /* Fast mode clock_div calculate: Tlow/Thigh = 2/1.*/
      chDbgAssert((STM32_PCLK1 % (clock_speed * 3)) == 0,
                  "i2c_lld_set_clock(), #5",
                  "PCLK1 must be divided without remainder");
      clock_div = (uint16_t)(STM32_PCLK1 / (clock_speed * 3));
    }
    else if (duty == FAST_DUTY_CYCLE_16_9) {
      /* Fast mode clock_div calculate: Tlow/Thigh = 16/9.*/
      chDbgAssert((STM32_PCLK1 % (clock_speed * 25)) == 0,
                  "i2c_lld_set_clock(), #6",
                  "PCLK1 must be divided without remainder");
      clock_div = (uint16_t)(STM32_PCLK1 / (clock_speed * 25));
      regCCR |= I2C_CCR_DUTY;
    }

    chDbgAssert(clock_div >= 0x01,
                    "i2c_lld_set_clock(), #7",
                    "Clock divider less then 0x04 not allowed");
    regCCR |= (I2C_CCR_FS | (clock_div & I2C_CCR_CCR));

    /* Sets the Maximum Rise Time for fast mode.*/
    dp->TRISE = (I2C_CLK_FREQ * 300 / 1000) + 1;
  }

  chDbgAssert((clock_div <= I2C_CCR_CCR),
              "i2c_lld_set_clock(), #8", "the selected clock is too low");

  dp->CCR = regCCR;
}

/**
 * @brief   Set operation mode of I2C hardware.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
static void i2c_lld_set_opmode(I2CDriver *i2cp) {
  I2C_TypeDef *dp = i2cp->i2c;
  i2copmode_t opmode = i2cp->config->op_mode;
  uint16_t regCR1;

  regCR1 = dp->CR1;
  switch (opmode) {
  case OPMODE_I2C:
    regCR1 &= (uint16_t)~(I2C_CR1_SMBUS|I2C_CR1_SMBTYPE);
    break;
  case OPMODE_SMBUS_DEVICE:
    regCR1 |= I2C_CR1_SMBUS;
    regCR1 &= (uint16_t)~(I2C_CR1_SMBTYPE);
    break;
  case OPMODE_SMBUS_HOST:
    regCR1 |= (I2C_CR1_SMBUS|I2C_CR1_SMBTYPE);
    break;
  }
  dp->CR1 = regCR1;
}

/**
 * @brief   I2C shared ISR code.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
static void i2c_lld_serve_event_interrupt(I2CDriver *i2cp) {

    I2C_TypeDef *dp = i2cp->i2c;
    uint32_t regSR = dp->SR2;
    uint32_t event = dp->SR1;

    #if I2C_USE_SLAVE_MODE
    if ( !i2cp->slave_mode )
    {
    #endif //I2C_USE_SLAVE_MODE
      /* Interrupts are disabled just before dmaStreamEnable() because there
         is no need of interrupts until next transaction begin. All the work is
         done by the DMA.*/
      switch (I2C_EV_MASK & (event | (regSR << 16))) {
      case I2C_EV5_MASTER_MODE_SELECT:
        dp->DR = i2cp->addr;
        break;
      case I2C_EV6_MASTER_REC_MODE_SELECTED:
        dp->CR2 &= ~I2C_CR2_ITEVTEN;
        dmaStreamEnable(i2cp->dmarx);
        dp->CR2 |= I2C_CR2_LAST;                 /* Needed in receiver mode. */
        if (dmaStreamGetTransactionSize(i2cp->dmarx) < 2)
          dp->CR1 &= ~I2C_CR1_ACK;
        break;
      case I2C_EV6_MASTER_TRA_MODE_SELECTED:
        dp->CR2 &= ~I2C_CR2_ITEVTEN;
        dmaStreamEnable(i2cp->dmatx);
        break;
      case I2C_EV8_2_MASTER_BYTE_TRANSMITTED:
        /* Catches BTF event after the end of transmission.*/
        if (dmaStreamGetTransactionSize(i2cp->dmarx) > 0) {
          /* Starts "read after write" operation, LSB = 1 -> receive.*/
          i2cp->addr |= 0x01;
          dp->CR1 |= I2C_CR1_START | I2C_CR1_ACK;
          return;
        }
        dp->CR2 &= ~I2C_CR2_ITEVTEN;
        dp->CR1 |= I2C_CR1_STOP;
        wakeup_isr(i2cp, RDY_OK);
        break;
      default:
        break;
      }
      /* Clear ADDR flag. */
      if (event & (I2C_SR1_ADDR | I2C_SR1_ADD10))
        (void)dp->SR2;
    #if I2C_USE_SLAVE_MODE
    }
    else
    {
        if  (event & (I2C_SR1_ADDR | I2C_SR1_ADD10))
        {
            // When transaction begins reset byte counters.
            i2cp->rxind = 0;
            i2cp->txind = 0;

            // Clear Addr Flag
            event = dp->SR1;
            regSR = dp->SR2;
        }
        if ( event & I2C_SR1_TXE )
        {
            dp->DR = i2cp->txbuf[ i2cp->txind++ ];
            if ( i2cp->rxind >= i2cp->txbytes )
                i2cp->txind = 0;
        }
        if ( event & I2C_SR1_RXNE )
        {
            i2cp->rxbuf[ i2cp->rxind++ ] = dp->DR;
            if ( i2cp->rxind >= i2cp->rxbytes )
                i2cp->rxind = 0;
        }
        if ( event & I2C_SR1_STOPF )
        {
            // Clear STOPF bit by writing to CR1.
            event = dp->SR1;
            dp->CR1 |= I2C_CR1_PE;

            // Notify user about receive finish.
            if ( i2cp->rxcb )
                i2cp->rxcb( i2cp );
        }
        if ( event & I2C_SR1_AF )
        {
            dp->SR1 &= ~I2C_SR1_AF;

            // Notify user about transfer finish.
            if ( i2cp->txcb )
                i2cp->txcb( i2cp );
        }
    }
    #endif // I2C_USE_SLAVE_MODE
}

/**
 * @brief   DMA RX end IRQ handler.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] flags     pre-shifted content of the ISR register
 *
 * @notapi
 */
static void i2c_lld_serve_rx_end_irq(I2CDriver *i2cp, uint32_t flags) {
  I2C_TypeDef *dp = i2cp->i2c;

  /* DMA errors handling.*/
#if defined(STM32_I2C_DMA_ERROR_HOOK)
  if ((flags & (STM32_DMA_ISR_TEIF | STM32_DMA_ISR_DMEIF)) != 0) {
    STM32_I2C_DMA_ERROR_HOOK(i2cp);
  }
#else
  (void)flags;
#endif

  dmaStreamDisable(i2cp->dmarx);

#if I2C_USE_SLAVE_MODE
  if ( !i2cp->slave_mode )
  {
#endif
      dp->CR2 &= ~I2C_CR2_LAST;
      dp->CR1 &= ~I2C_CR1_ACK;
      dp->CR1 |= I2C_CR1_STOP;
      wakeup_isr(i2cp, RDY_OK);
#if I2C_USE_SLAVE_MODE
  }
#endif
}

/**
 * @brief    DMA TX end IRQ handler.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
static void i2c_lld_serve_tx_end_irq(I2CDriver *i2cp, uint32_t flags) {
  I2C_TypeDef *dp = i2cp->i2c;

  /* DMA errors handling.*/
#if defined(STM32_I2C_DMA_ERROR_HOOK)
  if ((flags & (STM32_DMA_ISR_TEIF | STM32_DMA_ISR_DMEIF)) != 0) {
    STM32_I2C_DMA_ERROR_HOOK(i2cp);
  }
#else
  (void)flags;
#endif

  dmaStreamDisable(i2cp->dmatx);
  /* Enables interrupts to catch BTF event meaning transmission part complete.
     Interrupt handler will decide to generate STOP or to begin receiving part
     of R/W transaction itself.*/
  dp->CR2 |= I2C_CR2_ITEVTEN;
}

/**
 * @brief   I2C error handler.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
static void i2c_lld_serve_error_interrupt(I2CDriver *i2cp) {
  I2C_TypeDef *dp = i2cp->i2c;
  i2cflags_t errors;

  /* Clears interrupt flags just to be safe.*/
  chSysLockFromIsr();
  dmaStreamDisable(i2cp->dmatx);
  dmaStreamDisable(i2cp->dmarx);
  chSysUnlockFromIsr();

  errors = I2CD_NO_ERROR;

  if (dp->SR1 & I2C_SR1_BERR) {                     /* Bus error.           */
    dp->SR1 &= ~I2C_SR1_BERR;
    errors |= I2CD_BUS_ERROR;
  }
  if (dp->SR1 & I2C_SR1_ARLO) {                     /* Arbitration lost.    */
    dp->SR1 &= ~I2C_SR1_ARLO;
    errors |= I2CD_ARBITRATION_LOST;
  }
#if I2C_USE_SLAVE_MODE
  if ( !i2cp->slave_mode )
  {
#endif
  if (dp->SR1 & I2C_SR1_AF) {                       /* Acknowledge fail.    */
    dp->SR1 &= ~I2C_SR1_AF;
    dp->CR2 &= ~I2C_CR2_ITEVTEN;
    dp->CR1 |= I2C_CR1_STOP;                        /* Setting stop bit.    */
    errors |= I2CD_ACK_FAILURE;
  }
#if I2C_USE_SLAVE_MODE
  }
  else
  {
      // In slave mode it is not an error it is end of slave transfer.
      // Just clear AF flag.
      dp->SR1 &= ~I2C_SR1_AF;

      // Notify user about transfer finish.
      if ( i2cp->txcb )
          i2cp->txcb( i2cp );
  }
#endif
  if (dp->SR1 & I2C_SR1_OVR) {                      /* Overrun.             */
    dp->SR1 &= ~I2C_SR1_OVR;
    errors |= I2CD_OVERRUN;
  }
  if (dp->SR1 & I2C_SR1_PECERR) {                   /* PEC error.           */
    dp->SR1 &= ~I2C_SR1_PECERR;
    errors |= I2CD_PEC_ERROR;
  }
  if (dp->SR1 & I2C_SR1_TIMEOUT) {                  /* SMBus Timeout.       */
    dp->SR1 &= ~I2C_SR1_TIMEOUT;
    errors |= I2CD_TIMEOUT;
  }
  if (dp->SR1 & I2C_SR1_SMBALERT) {                 /* SMBus alert.         */
    dp->SR1 &= ~I2C_SR1_SMBALERT;
    errors |= I2CD_SMB_ALERT;
  }

  /* If some error has been identified then sends wakes the waiting thread.*/
  if (errors != I2CD_NO_ERROR) {
    i2cp->errors = errors;
    wakeup_isr(i2cp, RDY_RESET);
  }
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if STM32_I2C_USE_I2C1 || defined(__DOXYGEN__)
/**
 * @brief   I2C1 event interrupt handler.
 *
 * @notapi
 */
CH_IRQ_HANDLER(I2C1_EV_IRQHandler) {

  CH_IRQ_PROLOGUE();

  i2c_lld_serve_event_interrupt(&I2CD1);

  CH_IRQ_EPILOGUE();
}

/**
 * @brief   I2C1 error interrupt handler.
 */
CH_IRQ_HANDLER(I2C1_ER_IRQHandler) {

  CH_IRQ_PROLOGUE();

  i2c_lld_serve_error_interrupt(&I2CD1);

  CH_IRQ_EPILOGUE();
}
#endif /* STM32_I2C_USE_I2C1 */

#if STM32_I2C_USE_I2C2 || defined(__DOXYGEN__)
/**
 * @brief   I2C2 event interrupt handler.
 *
 * @notapi
 */
CH_IRQ_HANDLER(I2C2_EV_IRQHandler) {

  CH_IRQ_PROLOGUE();

  i2c_lld_serve_event_interrupt(&I2CD2);

  CH_IRQ_EPILOGUE();
}

/**
 * @brief   I2C2 error interrupt handler.
 *
 * @notapi
 */
CH_IRQ_HANDLER(I2C2_ER_IRQHandler) {

  CH_IRQ_PROLOGUE();

  i2c_lld_serve_error_interrupt(&I2CD2);

  CH_IRQ_EPILOGUE();
}
#endif /* STM32_I2C_USE_I2C2 */

#if STM32_I2C_USE_I2C3 || defined(__DOXYGEN__)
/**
 * @brief   I2C3 event interrupt handler.
 *
 * @notapi
 */
CH_IRQ_HANDLER(I2C3_EV_IRQHandler) {

  CH_IRQ_PROLOGUE();

  i2c_lld_serve_event_interrupt(&I2CD3);

  CH_IRQ_EPILOGUE();
}

/**
 * @brief   I2C3 error interrupt handler.
 *
 * @notapi
 */
CH_IRQ_HANDLER(I2C3_ER_IRQHandler) {

  CH_IRQ_PROLOGUE();

  i2c_lld_serve_error_interrupt(&I2CD3);

  CH_IRQ_EPILOGUE();
}
#endif /* STM32_I2C_USE_I2C3 */

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level I2C driver initialization.
 *
 * @notapi
 */
void i2c_lld_init(void) {

#if STM32_I2C_USE_I2C1
  i2cObjectInit(&I2CD1);
  I2CD1.thread = NULL;
  I2CD1.i2c    = I2C1;
  I2CD1.dmarx  = STM32_DMA_STREAM(STM32_I2C_I2C1_RX_DMA_STREAM);
  I2CD1.dmatx  = STM32_DMA_STREAM(STM32_I2C_I2C1_TX_DMA_STREAM);
#endif /* STM32_I2C_USE_I2C1 */

#if STM32_I2C_USE_I2C2
  i2cObjectInit(&I2CD2);
  I2CD2.thread = NULL;
  I2CD2.i2c    = I2C2;
  I2CD2.dmarx  = STM32_DMA_STREAM(STM32_I2C_I2C2_RX_DMA_STREAM);
  I2CD2.dmatx  = STM32_DMA_STREAM(STM32_I2C_I2C2_TX_DMA_STREAM);
#endif /* STM32_I2C_USE_I2C2 */

#if STM32_I2C_USE_I2C3
  i2cObjectInit(&I2CD3);
  I2CD3.thread = NULL;
  I2CD3.i2c    = I2C3;
  I2CD3.dmarx  = STM32_DMA_STREAM(STM32_I2C_I2C3_RX_DMA_STREAM);
  I2CD3.dmatx  = STM32_DMA_STREAM(STM32_I2C_I2C3_TX_DMA_STREAM);
#endif /* STM32_I2C_USE_I2C3 */
}

/**
 * @brief   Configures and activates the I2C peripheral.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
void i2c_lld_start(I2CDriver *i2cp) {
  I2C_TypeDef *dp = i2cp->i2c;

  i2cp->dmamode = STM32_DMA_CR_PSIZE_BYTE | STM32_DMA_CR_MSIZE_BYTE |
                  STM32_DMA_CR_MINC | STM32_DMA_CR_DMEIE | STM32_DMA_CR_TEIE |
                  STM32_DMA_CR_TCIE;

  /* If in stopped state then enables the I2C and DMA clocks.*/
  if (i2cp->state == I2C_STOP) {

#if STM32_I2C_USE_I2C1
    if (&I2CD1 == i2cp) {
      bool_t b;

      rccResetI2C1();
      b = dmaStreamAllocate(i2cp->dmarx,
                            STM32_I2C_I2C1_IRQ_PRIORITY,
                            (stm32_dmaisr_t)i2c_lld_serve_rx_end_irq,
                            (void *)i2cp);
      chDbgAssert(!b, "i2c_lld_start(), #1", "stream already allocated");
      b = dmaStreamAllocate(i2cp->dmatx,
                            STM32_I2C_I2C1_IRQ_PRIORITY,
                            (stm32_dmaisr_t)i2c_lld_serve_tx_end_irq,
                            (void *)i2cp);
      chDbgAssert(!b, "i2c_lld_start(), #2", "stream already allocated");
      rccEnableI2C1(FALSE);
      nvicEnableVector(I2C1_EV_IRQn,
          CORTEX_PRIORITY_MASK(STM32_I2C_I2C1_IRQ_PRIORITY));
      nvicEnableVector(I2C1_ER_IRQn,
          CORTEX_PRIORITY_MASK(STM32_I2C_I2C1_IRQ_PRIORITY));

      i2cp->dmamode |= STM32_DMA_CR_CHSEL(I2C1_RX_DMA_CHANNEL) |
                       STM32_DMA_CR_PL(STM32_I2C_I2C1_DMA_PRIORITY);
    }
#endif /* STM32_I2C_USE_I2C1 */

#if STM32_I2C_USE_I2C2
    if (&I2CD2 == i2cp) {
      bool_t b;

      rccResetI2C2();
      b = dmaStreamAllocate(i2cp->dmarx,
                            STM32_I2C_I2C2_IRQ_PRIORITY,
                            (stm32_dmaisr_t)i2c_lld_serve_rx_end_irq,
                            (void *)i2cp);
      chDbgAssert(!b, "i2c_lld_start(), #3", "stream already allocated");
      b = dmaStreamAllocate(i2cp->dmatx,
                            STM32_I2C_I2C2_IRQ_PRIORITY,
                            (stm32_dmaisr_t)i2c_lld_serve_tx_end_irq,
                            (void *)i2cp);
      chDbgAssert(!b, "i2c_lld_start(), #4", "stream already allocated");
      rccEnableI2C2(FALSE);
      nvicEnableVector(I2C2_EV_IRQn,
          CORTEX_PRIORITY_MASK(STM32_I2C_I2C2_IRQ_PRIORITY));
      nvicEnableVector(I2C2_ER_IRQn,
          CORTEX_PRIORITY_MASK(STM32_I2C_I2C2_IRQ_PRIORITY));

      i2cp->dmamode |= STM32_DMA_CR_CHSEL(I2C2_RX_DMA_CHANNEL) |
                       STM32_DMA_CR_PL(STM32_I2C_I2C2_DMA_PRIORITY);
    }
#endif /* STM32_I2C_USE_I2C2 */

#if STM32_I2C_USE_I2C3
    if (&I2CD3 == i2cp) {
      bool_t b;

      rccResetI2C3();
      b = dmaStreamAllocate(i2cp->dmarx,
                            STM32_I2C_I2C3_IRQ_PRIORITY,
                            (stm32_dmaisr_t)i2c_lld_serve_rx_end_irq,
                            (void *)i2cp);
      chDbgAssert(!b, "i2c_lld_start(), #5", "stream already allocated");
      b = dmaStreamAllocate(i2cp->dmatx,
                            STM32_I2C_I2C3_IRQ_PRIORITY,
                            (stm32_dmaisr_t)i2c_lld_serve_tx_end_irq,
                            (void *)i2cp);
      chDbgAssert(!b, "i2c_lld_start(), #6", "stream already allocated");
      rccEnableI2C3(FALSE);
      nvicEnableVector(I2C3_EV_IRQn,
          CORTEX_PRIORITY_MASK(STM32_I2C_I2C3_IRQ_PRIORITY));
      nvicEnableVector(I2C3_ER_IRQn,
          CORTEX_PRIORITY_MASK(STM32_I2C_I2C3_IRQ_PRIORITY));

      i2cp->dmamode |= STM32_DMA_CR_CHSEL(I2C3_RX_DMA_CHANNEL) |
                       STM32_DMA_CR_PL(STM32_I2C_I2C3_DMA_PRIORITY);
    }
#endif /* STM32_I2C_USE_I2C3 */
  }

  /* DMA streams mode preparation in advance.*/
  dmaStreamSetMode(i2cp->dmatx, i2cp->dmamode | STM32_DMA_CR_DIR_M2P);
  dmaStreamSetMode(i2cp->dmarx, i2cp->dmamode | STM32_DMA_CR_DIR_P2M);

  /* I2C registers pointed by the DMA.*/
  dmaStreamSetPeripheral(i2cp->dmarx, &dp->DR);
  dmaStreamSetPeripheral(i2cp->dmatx, &dp->DR);

  /* Reset i2c peripheral.*/
  dp->CR1 = I2C_CR1_SWRST;
  dp->CR1 = 0;
  dp->CR2 = I2C_CR2_ITERREN | I2C_CR2_DMAEN;

  /* Setup I2C parameters.*/
  i2c_lld_set_clock(i2cp);
  i2c_lld_set_opmode(i2cp);

  /* Ready to go.*/
  dp->CR1 |= I2C_CR1_PE;
}

/**
 * @brief   Deactivates the I2C peripheral.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
void i2c_lld_stop(I2CDriver *i2cp) {

  /* If not in stopped state then disables the I2C clock.*/
  if (i2cp->state != I2C_STOP) {

    /* I2C disable.*/
    i2c_lld_abort_operation(i2cp);
    dmaStreamRelease(i2cp->dmatx);
    dmaStreamRelease(i2cp->dmarx);

#if STM32_I2C_USE_I2C1
    if (&I2CD1 == i2cp) {
      nvicDisableVector(I2C1_EV_IRQn);
      nvicDisableVector(I2C1_ER_IRQn);
      rccDisableI2C1(FALSE);
    }
#endif

#if STM32_I2C_USE_I2C2
    if (&I2CD2 == i2cp) {
      nvicDisableVector(I2C2_EV_IRQn);
      nvicDisableVector(I2C2_ER_IRQn);
      rccDisableI2C2(FALSE);
    }
#endif

#if STM32_I2C_USE_I2C3
    if (&I2CD3 == i2cp) {
      nvicDisableVector(I2C3_EV_IRQn);
      nvicDisableVector(I2C3_ER_IRQn);
      rccDisableI2C3(FALSE);
    }
#endif
  }
}

/**
 * @brief   Receives data via the I2C bus as master.
 * @details Number of receiving bytes must be more than 1 because of stm32
 *          hardware restrictions.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] addr      slave device address
 * @param[out] rxbuf    pointer to the receive buffer
 * @param[in] rxbytes   number of bytes to be received
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval RDY_OK       if the function succeeded.
 * @retval RDY_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval RDY_TIMEOUT  if a timeout occurred before operation end. <b>After a
 *                      timeout the driver must be stopped and restarted
 *                      because the bus is in an uncertain state</b>.
 *
 * @notapi
 */
msg_t i2c_lld_master_receive_timeout(I2CDriver *i2cp, i2caddr_t addr,
                                     uint8_t *rxbuf, size_t rxbytes,
                                     systime_t timeout) {
  I2C_TypeDef *dp = i2cp->i2c;
  VirtualTimer vt;

#if defined(STM32F1XX_I2C)
  chDbgCheck((rxbytes > 1), "i2c_lld_master_receive_timeout");
#endif

  /* Global timeout for the whole operation.*/
  if (timeout != TIME_INFINITE)
    chVTSetI(&vt, timeout, i2c_lld_safety_timeout, (void *)i2cp);

  /* Releases the lock from high level driver.*/
  chSysUnlock();

  /* Initializes driver fields, LSB = 1 -> receive.*/
  i2cp->addr = (addr << 1) | 0x01;
  i2cp->errors = 0;

  /* RX DMA setup.*/
  dmaStreamSetMemory0(i2cp->dmarx, rxbuf);
  dmaStreamSetTransactionSize(i2cp->dmarx, rxbytes);

  /* Waits until BUSY flag is reset and the STOP from the previous operation
     is completed, alternatively for a timeout condition.*/
  while ((dp->SR2 & I2C_SR2_BUSY) || (dp->CR1 & I2C_CR1_STOP)) {
    chSysLock();
    if ((timeout != TIME_INFINITE) && !chVTIsArmedI(&vt))
      return RDY_TIMEOUT;
    chSysUnlock();
  }

  /* This lock will be released in high level driver.*/
  chSysLock();

  /* Atomic check on the timer in order to make sure that a timeout didn't
     happen outside the critical zone.*/
  if ((timeout != TIME_INFINITE) && !chVTIsArmedI(&vt))
    return RDY_TIMEOUT;

#if I2C_USE_SLAVE_MODE
  i2cp->slave_mode = 0;
#endif
  /* Starts the operation.*/
  dp->CR2 |= I2C_CR2_ITEVTEN;
  dp->CR1 |= I2C_CR1_START | I2C_CR1_ACK;

  /* Waits for the operation completion or a timeout.*/
  i2cp->thread = chThdSelf();
  chSchGoSleepS(THD_STATE_SUSPENDED);
  if ((timeout != TIME_INFINITE) && chVTIsArmedI(&vt))
    chVTResetI(&vt);

  return chThdSelf()->p_u.rdymsg;
}

/**
 * @brief   Transmits data via the I2C bus as master.
 * @details Number of receiving bytes must be 0 or more than 1 because of stm32
 *          hardware restrictions.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] addr      slave device address
 * @param[in] txbuf     pointer to the transmit buffer
 * @param[in] txbytes   number of bytes to be transmitted
 * @param[out] rxbuf    pointer to the receive buffer
 * @param[in] rxbytes   number of bytes to be received
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval RDY_OK       if the function succeeded.
 * @retval RDY_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval RDY_TIMEOUT  if a timeout occurred before operation end. <b>After a
 *                      timeout the driver must be stopped and restarted
 *                      because the bus is in an uncertain state</b>.
 *
 * @notapi
 */
msg_t i2c_lld_master_transmit_timeout(I2CDriver *i2cp, i2caddr_t addr,
                                      const uint8_t *txbuf, size_t txbytes,
                                      uint8_t *rxbuf, size_t rxbytes,
                                      systime_t timeout) {
  I2C_TypeDef *dp = i2cp->i2c;
  VirtualTimer vt;

#if defined(STM32F1XX_I2C)
  chDbgCheck(((rxbytes == 0) || ((rxbytes > 1) && (rxbuf != NULL))),
             "i2c_lld_master_transmit_timeout");
#endif

  /* Global timeout for the whole operation.*/
  if (timeout != TIME_INFINITE)
    chVTSetI(&vt, timeout, i2c_lld_safety_timeout, (void *)i2cp);

  /* Releases the lock from high level driver.*/
  chSysUnlock();

  /* Initializes driver fields, LSB = 0 -> write.*/
  i2cp->addr = addr << 1;
  i2cp->errors = 0;

  /* TX DMA setup.*/
  dmaStreamSetMemory0(i2cp->dmatx, txbuf);
  dmaStreamSetTransactionSize(i2cp->dmatx, txbytes);

  /* RX DMA setup.*/
  dmaStreamSetMemory0(i2cp->dmarx, rxbuf);
  dmaStreamSetTransactionSize(i2cp->dmarx, rxbytes);

  /* Waits until BUSY flag is reset and the STOP from the previous operation
     is completed, alternatively for a timeout condition.*/
  while ((dp->SR2 & I2C_SR2_BUSY) || (dp->CR1 & I2C_CR1_STOP)) {
    chSysLock();
    if ((timeout != TIME_INFINITE) && !chVTIsArmedI(&vt))
      return RDY_TIMEOUT;
    chSysUnlock();
  }

  /* This lock will be released in high level driver.*/
  chSysLock();

  /* Atomic check on the timer in order to make sure that a timeout didn't
     happen outside the critical zone.*/
  if ((timeout != TIME_INFINITE) && !chVTIsArmedI(&vt))
    return RDY_TIMEOUT;

#if I2C_USE_SLAVE_MODE
  i2cp->slave_mode = 0;
#endif
  /* Starts the operation.*/
  dp->CR2 |= I2C_CR2_ITEVTEN;
  dp->CR1 |= I2C_CR1_START;

  /* Waits for the operation completion or a timeout.*/
  i2cp->thread = chThdSelf();
  chSchGoSleepS(THD_STATE_SUSPENDED);
  if ((timeout != TIME_INFINITE) && chVTIsArmedI(&vt))
    chVTResetI(&vt);

  return chThdSelf()->p_u.rdymsg;
}

#if I2C_USE_SLAVE_MODE

msg_t i2c_lld_slave_io_timeout( I2CDriver *i2cp, i2caddr_t addr,
                                uint8_t * rxbuf, size_t rxbytes,
                                uint8_t * txbuf, size_t txbytes,
                                TI2cSlaveCb rxcb,
                                TI2cSlaveCb txcb,
                                systime_t timeout )
{
    I2C_TypeDef *dp = i2cp->i2c;
    VirtualTimer vt;

    // Global timeout for the whole operation.
    i2cp->thread = chThdSelf();
    if (timeout != TIME_INFINITE)
      chVTSetI(&vt, timeout, i2c_lld_safety_timeout, (void *)i2cp);

    // Waits until BUSY flag is reset and the STOP from the previous operation
    // is completed, alternatively for a timeout condition.
    chSysUnlock();
    while ( (dp->SR2 & I2C_SR2_BUSY) || ( dp->CR1 & I2C_CR1_STOP ) )
    {
        chSysLock();
        if ((timeout != TIME_INFINITE) && !chVTIsArmedI(&vt))
            return RDY_TIMEOUT;
        chSysUnlock();
    }

    // This lock will be released in high level driver.
    chSysLock();

    // Atomic check on the timer in order to make sure that a timeout didn't
    // happen outside the critical zone.
    if ((timeout != TIME_INFINITE) && !chVTIsArmedI(&vt))
        return RDY_TIMEOUT;

    // Initializes driver fields, LSB = 1 -> read.
    i2cp->addr    = (addr << 1);
    i2cp->errors  = 0;

    i2cp->rxbuf   = rxbuf;
    i2cp->rxbytes = rxbytes;
    i2cp->rxind = 0;
    i2cp->rxcb  = rxcb;

    i2cp->txbuf   = txbuf;
    i2cp->txbytes = txbytes;
    i2cp->txind = 0;
    i2cp->txcb  = txcb;

    i2cp->slave_mode = 1;
    // Starts the operation.
    // No start - slave mode.
    dp->CR1 &= ~( I2C_CR1_START );
    // Turn off ISR and DMA.
    dp->CR2 &= ~( I2C_CR2_DMAEN );
    // Own address.
    dp->OAR1 = ((addr << 1) & (0xFE));
    // Turn interrupts and buffer interrupts on.
    dp->CR2 |= (I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
    // Generate Ack on address match and IOs.
    dp->CR1 |= I2C_CR1_ACK;

    /* Waits for the operation completion or a timeout.*/
    i2cp->thread = chThdSelf();
    //chSchGoSleepS( THD_STATE_SUSPENDED );
    if ( ( timeout != TIME_INFINITE ) && chVTIsArmedI( &vt ) )
      chVTResetI( &vt );

    //return chThdSelf()->p_u.rdymsg;
    return RDY_OK;
}


#endif


#endif /* HAL_USE_I2C */

/** @} */
