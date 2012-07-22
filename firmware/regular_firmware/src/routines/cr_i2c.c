
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

    	static uint8_t one = 0;
    	if ( one == 0 )
    	{
    		crDELAY( xHandle, 1 );
            i2cSetEn( 0, 1 );
            i2cConfig( 0, 1, 0, 10000 );
            uint8_t buf[3];
            buf[0] = 0;
            buf[1] = 0x0;
            buf[2] = 0xEA;
            i2cIo( 0, 0xA0 + 0, 1, 1, buf );
            one = 1;
        }


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
						data = idc->sendQueue[ i ];
						I2C_SendData( idc->i2c, data );

						// Test for TXE flag (data sent).
						idc->status = I2C_MBT;
						idc->elapsed = 0;
						while (  !I2C_CheckEvent( idc->i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
						{
							if ( idc->elapsed++ > idc->timeout )
						    {
							    idc->status = I2C_ERROR_MBT;
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
							goto i2c_end;
						}
						crDELAY( xHandle, 1 );
						idc = i2c( uxIndex );
					}

					// Receiving a number of bytes from slave.
					uint8_t i;
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
            idc->status = I2C_IDLE;
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




