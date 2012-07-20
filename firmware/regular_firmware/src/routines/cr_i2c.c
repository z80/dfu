
#include "cr_i2c.h"
#include "i2c.h"
#include "config.h"
#include "stm32f10x.h"

void crI2c( xCoRoutineHandle xHandle, 
            unsigned portBASE_TYPE uxIndex )
{
    static uint32_t timeout;
    crSTART( xHandle );
    for ( ;; )
    {
        // No should be initialized in main() before cocroutine start.
        // Check if uninitialized.
        /*if ( ( uxIndex == 0 ) && ( !g_initialized0 ) )
        {
            i2cInit( 0 );
	    g_initialized0 = 1;
        }
		else if ( ( uxIndex == 1 ) && ( !g_initialized1 ) )
		{
			i2cInit( 1 );
			g_initialized
		}*/

		//g_status = I2C_IDLE;

        i2cConfig( 0, 1, 0, 1000 );
        i2cSetEn( 0, 1 );
        uint8_t buf[3];
        buf[0] = 0;
        buf[1] = 0xAE;
        buf[2] = 0xEA;
        i2cIo( 0, 0xA0 + 0, 1, 0, buf );
		TI2C * idc = i2c( uxIndex );
		// Commands loop.
		if ( idc->master )
		{
			idc->status = I2C_MASTER_IO;
			if ( ( idc->sendCnt ) || ( idc->receiveCnt ) )
			{
				idc->status = I2C_ENTERED_IO;
				idc->status = I2C_WAIT_BUSY;
				// wait for BUSY bit to get cleared.
				idc->elapsed = 0;
				while ( I2C_GetFlagStatus( idc->i2c, I2C_FLAG_BUSY ) )
				{
					if ( idc->elapsed++ > idc->timeout )
				    {
					    idc->status = I2C_ERROR;
					    goto i2c_end;
				    }
					crDELAY( xHandle, 1 );
					idc = i2c( uxIndex );
				}

		        I2C_AcknowledgeConfig( i2c, ENABLE );

				// Generate START condition on a bus.
				I2C_GenerateSTART( idc->i2c, ENABLE );

				// Wait for SB to be set
				timeout = 0;
				while ( I2C_GetFlagStatus( idc->i2c, I2C_FLAG_SB ) == RESET )
				{
					if ( idc->elapsed++ > idc->timeout )
					{
						idc->status = I2C_ERROR;
						goto i2c_end;
					}
					crDELAY( xHandle, 1 );
					idc = i2c( uxIndex );
				}

				if ( idc->sendCnt )
				{
					// Transmit the slave address with write operation enabled.
					I2C_Send7bitAddress( idc->i2c, idc->address, I2C_Direction_Transmitter );

					// Test on ADDR flag.
					idc->elapsed = 0;
					while (  !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) )
					{
						if ( idc->elapsed++ > idc->timeout )
					    {
						    idc->status = I2C_ERROR;
						    goto i2c_end;
					    }
					    crDELAY( xHandle, 1 );
					    idc = i2c( uxIndex );
					}

						// Read data from send queue.
					uint8_t data, i;
					//portBASE_TYPE  rc;
					//crQUEUE_RECEIVE( xHandle, idc->sendQueue, &data, 0, &rc );
					//if ( rc == pdPASS )
					for ( i=0; i<idc->sendCnt; i++ )
					{
						idc = i2c( uxIndex );
						// Transmit data.
						data = idc->sendQueue[i];
						I2C_SendData( idc->i2c, data );

						// Test for TXE flag (data sent).
						idc->elapsed = 0;
						while ( ( !I2C_GetFlagStatus( idc->i2c, I2C_FLAG_TXE ) ) &&
					            ( !I2C_GetFlagStatus( idc->i2c, I2C_FLAG_BTF ) ) )
						{
							if ( idc->elapsed++ > idc->timeout )
						    {
							    idc->status = I2C_ERROR;
							    goto i2c_end;
							}
							crDELAY( xHandle, 1 );
						    idc = i2c( uxIndex );
						}
					}

					// Wait untill BTF flag is set before generating STOP.
					idc->elapsed = 0;
					while ( I2C_GetFlagStatus( idc->i2c, I2C_FLAG_BTF ) )
					{
						if ( idc->elapsed++ > idc->timeout )
						{
						idc->status = I2C_ERROR;
						goto i2c_end;
					}
						crDELAY( xHandle, 1 );
					idc = i2c( uxIndex );
					}
				}
				// Receiving data if necessary.
				if ( idc->receiveCnt )
				{
					if ( idc->sendCnt )
					{
						// Generate START condition if there was at least one byte written.
						I2C_GenerateSTART( idc->i2c, ENABLE );
						// Wait for SB flag.
						idc->elapsed = 0;
						while ( !I2C_GetFlagStatus( idc->i2c, I2C_FLAG_SB ) )
						{
							if ( idc->elapsed++ > idc->timeout )
							{
								idc->status = I2C_ERROR;
							goto i2c_end;
							}
							crDELAY( xHandle, 1 );
							idc = i2c( uxIndex );
						}

						I2C_Send7bitAddress( idc->i2c, idc->address, I2C_Direction_Receiver );

						// Test on ADDR Flag
						idc->timeout = 0;
						while ( !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) )
						{
							if ( idc->elapsed++ > idc->timeout )
							{
								idc->status = I2C_ERROR;
								goto i2c_end;
							}
							crDELAY( xHandle, 1 );
							idc = i2c( uxIndex );
						}

						// Receiving a number of bytes from slave.
						uint8_t i;
						for ( i=0; i<idc->receiveCnt; i++ )
						{
							// Wait for data available.
							idc->elapsed = 0;
							while ( !I2C_GetFlagStatus( idc->i2c, I2C_FLAG_RXNE ) )
							{
								if ( idc->elapsed++ > idc->timeout )
							{
								idc->status = I2C_ERROR;
								goto i2c_end;
							}
								crDELAY( xHandle, 1 );
								idc = i2c( uxIndex );
							}

							// Read the data.
							uint8_t data = I2C_ReceiveData( idc->i2c );

							// Send data through queue.
								//portBASE_TYPE  rc;
										//crQUEUE_SEND( xHandle, idc->receiveQueue, &data, 0, &rc );
							//if ( rc != pdPASS )
							//{
							//    idc->status = I2C_ERROR;
							//      goto i2c_end;
							//}
							idc->receiveQueue[i] = data;
						}

						// Send STOP Condition
						//I2C_GenerateSTOP( idc->i2c, ENABLE );
					}
				}

				// Generating STOP.
				I2C_GenerateSTOP( idc->i2c, ENABLE );

				// To prevent cyclic writes of zero data.
			   idc->sendCnt = 0;
			   idc->receiveCnt = 0;
			}
		}
		else // master.
		{
			// slave mode IO.
		}
		//idc->status = I2C_IDLE;
i2c_end:
		crDELAY( xHandle, 1 );
    }
    crEND();
}




