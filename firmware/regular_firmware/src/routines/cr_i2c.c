
#include "cr_i2c.h"
#include "i2c.h"
#include "config.h"
#include "stm32f10x.h"

void crI2c( xCoRoutineHandle xHandle,
            unsigned portBASE_TYPE uxIndex )
{
	static uint8_t i;
    crSTART( xHandle );
    for ( ;; )
    {

    	static uint8_t init = 0;
    	if ( init == 0 )
    	{
            i2cSetEn( 0, 1 );
            i2cConfig( 0, 0, 123, 10000 );
            init = 1;
        }

        TI2C * idc = i2c( uxIndex );

		// Commands loop.
		if ( idc->master )
		{
			if ( ( idc->sendCnt ) || ( idc->receiveCnt ) )
			{
				idc->status = I2C_BUSY;
				// wait for BUSY bit to get cleared.
				idc->elapsed = 0;
				while ( I2C_GetFlagStatus( idc->i2c, I2C_FLAG_BUSY ) )
				{
					if ( idc->elapsed++ > idc->timeout )
				    {
					    idc->status = I2C_ERROR_BUSY;
					    goto i2c_end;
				    }
					crDELAY( xHandle, 1 );
					idc = i2c( uxIndex );
				}

		        if ( idc->sendCnt )
				{
					// Generate START condition on a bus.
					I2C_GenerateSTART( idc->i2c, ENABLE );

					idc->status = I2C_MMS;
					// Wait for SB to be set
					idc->elapsed = 0;
					while ( !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_MODE_SELECT ) )
					{
						if ( idc->elapsed++ > idc->timeout )
						{
							idc->status = I2C_ERROR_MMS;
                            I2C_GenerateSTOP( idc->i2c, ENABLE );
							goto i2c_end;
						}
						crDELAY( xHandle, 1 );
						idc = i2c( uxIndex );
					}

					// Transmit the slave address with write operation enabled.
					I2C_Send7bitAddress( idc->i2c, idc->address, I2C_Direction_Transmitter );

					idc->status = I2C_TMS;
					// Test on ADDR flag.
					idc->elapsed = 0;
					while (  !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) )
					{
						if ( idc->elapsed++ > idc->timeout )
					    {
						    idc->status = I2C_ERROR_TMS;
                            I2C_GenerateSTOP( idc->i2c, ENABLE );
						    goto i2c_end;
					    }
					    crDELAY( xHandle, 1 );
					    idc = i2c( uxIndex );
					}


                    // Read data from send queue.
					for ( i=0; i<idc->sendCnt; i++ )
					{
						idc = i2c( uxIndex );
						// Transmit data.
						uint8_t data = idc->sendQueue[ i ];
						I2C_SendData( idc->i2c, data );

						// Test for TXE flag (data sent).
						idc->status = I2C_MBT;
						idc->elapsed = 0;
						while (  !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
						{
							if ( idc->elapsed++ > idc->timeout )
						    {
							    idc->status = I2C_ERROR_MBT;
                                I2C_GenerateSTOP( idc->i2c, ENABLE );
							    goto i2c_end;
							}
							crDELAY( xHandle, 1 );
						    idc = i2c( uxIndex );
						}
						idc->bytesWritten = i+1;
					}

					// Wait untill BTF flag is set before generating STOP.
					idc->status = I2C_BTF;
					idc->elapsed = 0;
					while ( !I2C_GetFlagStatus( idc->i2c, I2C_FLAG_BTF ) )
					{
						if ( idc->elapsed++ > idc->timeout )
                        {
						    idc->status = I2C_ERROR_BTF;
                            I2C_GenerateSTOP( idc->i2c, ENABLE );
						    goto i2c_end;
					    }
						crDELAY( xHandle, 1 );
					    idc = i2c( uxIndex );
					}
				}
				// Receiving data if necessary.
				if ( idc->receiveCnt )
				{
				    I2C_AcknowledgeConfig( idc->i2c, ENABLE );
					// Generate START condition if there was at least one byte written.
					I2C_GenerateSTART( idc->i2c, ENABLE );

					idc->status = I2C_MMS_R;
					// Wait for SB to be set
					idc->elapsed = 0;
					while ( !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_MODE_SELECT ) )
					{
						if ( idc->elapsed++ > idc->timeout )
						{
							idc->status = I2C_ERROR_MMS_R;
                            I2C_GenerateSTOP( idc->i2c, ENABLE );
							goto i2c_end;
						}
						crDELAY( xHandle, 1 );
						idc = i2c( uxIndex );
					}

					I2C_Send7bitAddress( idc->i2c, idc->address, I2C_Direction_Receiver );

					// Test on ADDR Flag
					idc->status = I2C_RMS;
					idc->elapsed = 0;
					while ( !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) )
					{
						if ( idc->elapsed++ > idc->timeout )
						{
							idc->status = I2C_ERROR_RMS;
                            I2C_GenerateSTOP( idc->i2c, ENABLE );
							goto i2c_end;
						}
						crDELAY( xHandle, 1 );
						idc = i2c( uxIndex );
					}

					// Receiving a number of bytes from slave.
					for ( i=0; i<idc->receiveCnt; i++ )
					{
						// Turn acknowledge off when reading the last byte.
						if ( i == (idc->receiveCnt-1) )
							I2C_AcknowledgeConfig( idc->i2c, DISABLE );
						// Wait for data available.
						idc->status = I2C_MBR;
						idc->elapsed = 0;
						while ( !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_BYTE_RECEIVED ) )
						{
							if ( idc->elapsed++ > idc->timeout )
                            {
							    idc->status = I2C_ERROR_MBR;
	                            I2C_GenerateSTOP( idc->i2c, ENABLE );
 						        goto i2c_end;
						    }
							crDELAY( xHandle, 1 );
							idc = i2c( uxIndex );
						}

						// Read the data.
						uint8_t data = I2C_ReceiveData( idc->i2c );
						idc->receiveQueue[i] = data;
						idc->bytesRead = i+1;
					}
				}

				// Generating STOP.
				I2C_GenerateSTOP( idc->i2c, ENABLE );
                // Idle status when finished in regular way.
                idc->status = I2C_IDLE;
			}
		}
		else // master.
		{
			// slave mode IO.
			//...... implementation.....
            // Idle status when finished in regular way.
            //idc->status = I2C_IDLE;
			crDELAY( xHandle, 1 );
		}
i2c_end:
		// To prevent cyclic writes of zero data.
		idc->sendCnt = 0;
		idc->receiveCnt = 0;
		// Give other coroutines time for running.
		crDELAY( xHandle, 1 );
    }
    crEND();
}




