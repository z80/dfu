
#include "cr_i2c.h"
#include "i2c.h"
#include "config.h"
#include "stm32f10x.h"

void crI2c( xCoRoutineHandle xHandle, 
            unsigned portBASE_TYPE uxIndex )
{
    crSTART( xHandle );
    for ( ;; )
    {
		//g_status = I2C_IDLE;

        i2cConfig( 0, 1, 0, 1000 );
        i2cSetEn( 0, 1 );
        uint8_t buf[3];
        buf[0] = 0;
        buf[1] = 0xAE;
        buf[2] = 0xEA;
        i2cIo( 0, 0xA0 + 0, 1, 1, buf );
        TI2C * idc = i2c( uxIndex );

        /*
        // Send START condition
        I2C_GenerateSTART( idc->i2c, ENABLE );

        // Test on EV5 and clear it
        while(!I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_MODE_SELECT ) );

        // Send EEPROM address for write
        I2C_Send7bitAddress( idc->i2c, 0xA0, I2C_Direction_Transmitter );

        // Test on EV6 and clear it
        while(!I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );


        // Send the EEPROM's internal address to write to : MSB of the address first
        I2C_SendData( idc->i2c,  12 );

        // Test on EV8 and clear it
        while( !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );



        // Send the EEPROM's internal address to write to : LSB of the address
        I2C_SendData( idc->i2c, 13 );

        // Test on EV8 and clear it
        while(! I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );


        I2C_SendData(idc->i2c, 12 );

        // Test on EV8 and clear it
        while ( !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );


        // Send STOP condition
        I2C_GenerateSTOP( idc->i2c, ENABLE );


        continue;
        */

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

				// Generate START condition on a bus.
				I2C_GenerateSTART( idc->i2c, ENABLE );

				// Wait for SB to be set
				idc->elapsed = 0;
				while ( !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_MODE_SELECT ) )
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
					for ( i=0; i<idc->sendCnt; i++ )
					{
						idc = i2c( uxIndex );
						// Transmit data.
						data = idc->sendQueue[i];
						I2C_SendData( idc->i2c, data );

						// Test for TXE flag (data sent).
						idc->elapsed = 0;
						while (  !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
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
					while ( !I2C_GetFlagStatus( idc->i2c, I2C_FLAG_BTF ) )
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
				        I2C_AcknowledgeConfig( idc->i2c, ENABLE );
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
						idc->elapsed = 0;
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
							if ( i == (idc->receiveCnt-1) )
								I2C_AcknowledgeConfig( idc->i2c, DISABLE );
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
							idc->receiveQueue[i] = data;
						}
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
		idc->status = I2C_IDLE;
i2c_end:
		crDELAY( xHandle, 1 );
    }
    crEND();
}




