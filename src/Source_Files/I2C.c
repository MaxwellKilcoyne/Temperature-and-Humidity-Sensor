/**
 * @file I2C.C
 * @author Max kilcoyne
 * @date 09/15/22
 * @brief The driver for I2C
 *
 */

#include "I2C.h"





typedef struct{
  DEFINED_STATES  current_state;
  I2C_TypeDef *I2Cx;
  bool ifBusy;
  uint32_t deviceAddress; // helper function sets
  bool read; // 1 is writing and 0 is reading
  uint32_t *bufferAddress; // store read or write buffer address
  uint32_t byteLefts; // helper function sets
  uint32_t I2C_CallBackEvent; // helper function sets
  uint32_t command; // helper function
  uint32_t data;
  int       numCmdBytes;
  uint32_t combinedBytes;

}I2C_STATE_MACHINE;



I2C_STATE_MACHINE i2c0;
I2C_STATE_MACHINE i2c1;

void ACK_Interrupt(I2C_STATE_MACHINE *i2c_sm);
void NACK_Interrupt(I2C_STATE_MACHINE *i2c_sm);
void MSTOP_Interrupt(I2C_STATE_MACHINE *i2c_sm);
void RXDATAV_Interrupt(I2C_STATE_MACHINE *i2c_sm);


/***************************************************************************/
/**
 * @brief
 *   I2C bus reset function
 *
 * @details
 *   This function use the I2C bus to reset all the connected devices, forcing them into a known state
 *   Sends out 9 clock pulses using the
 *
 * @note
 *   This function is used to reset the I2C bus and to
 *
 * @param[in] i2c pointer
 *   Pointer to the base peripheral address of the i2c peripheral being used
 *
 *
 ******************************************************************************/

static void i2cx_bus_reset(I2C_TypeDef *i2c){
 //directions are on lab 5 part 1 page 14
  uint32_t IENFlags;
  i2c->CMD = I2C_CMD_ABORT; // abort all stuff, so we can do stuff to the state machine
  IENFlags = i2c->IEN; // get the flags
  i2c -> IEN = _I2C_IEN_RESETVALUE; // disable all flags
  i2c->IFC = i2c->IF;
  i2c->CMD = I2C_CMD_START | I2C_CMD_STOP; // do the reset
//  I2C_IntClear(i2c, i2c -> IF); // clear flags again to clear reset flags
  while(!(i2c->IF & I2C_IF_MSTOP)); //as soon as the flag goes to 0, then we can proceed
  I2C_IntClear(i2c, i2c -> IF); // clear flags again to clear reset flags
  i2c->CMD = I2C_CMD_ABORT; // abort all stuff, to make sure it can be in a normal state
  i2c -> IEN = IENFlags;

}

/***************************************************************************/
/**
 * @brief
 *   I2C open function
 *
 * @details
 *  This function is used to initialize the values of the i2c that is being used, such as:
 *  The clock, the routing locations, the interrupts being used, etc.
 *
 * @note
 *
 * @param[in] i2c pointer
 * A pointer to the I2C peripheral, and a pointer to an I2C_Open_Struct_TypeDef
 *
 *
 ******************************************************************************/
void i2c_open(I2C_TypeDef *i2c_v, I2C_OPEN_STRUCT_TypeDef *I2C_T){

  I2C_Init_TypeDef I2C_values;

  if(i2c_v == I2C0){
      CMU_ClockEnable(cmuClock_I2C0, true);
  }
  if(i2c_v == I2C1){
      CMU_ClockEnable(cmuClock_I2C1, true);
  }

  if((i2c_v->IF & 0x01) == 0) {

      i2c_v->IFS = 0x01;
      EFM_ASSERT(i2c_v->IF & 0x01);
      i2c_v->IFC = 0x01;
  }
  else {
      i2c_v->IFC = 0x01;
      EFM_ASSERT(!(i2c_v->IF & 0x01));
  }

I2C_values.clhr = I2C_T->clhr;
I2C_values.enable = I2C_T->enable;
I2C_values.freq = I2C_T->freq;
I2C_values.master = I2C_T->master;
I2C_values.refFreq = I2C_T->refFreq;

I2C_Init(i2c_v, &I2C_values);

I2C_IntClear(i2c_v, _I2C_IF_MASK);


i2c_v->IEN = I2C_IEN_ACK;
i2c_v->IEN |= I2C_IEN_NACK;
i2c_v->IEN |= I2C_IEN_MSTOP;
i2c_v->IEN |= I2C_IEN_RXDATAV;

i2c_v->ROUTELOC0 =  I2C_T->ROUTEscl | I2C_T->ROUTEsda;
//i2c_v->ROUTELOC0 |= I2C_T->ROUTEsda;
i2c_v->ROUTEPEN = I2C_ROUTEPEN_SCLPEN * I2C_T->scl_pin_en;
i2c_v->ROUTEPEN |= I2C_ROUTEPEN_SDAPEN * I2C_T->sda_pin_en;

//i2cx_bus_reset(i2c_v);

if (i2c_v == I2C0) {
    NVIC_EnableIRQ(I2C0_IRQn);
    i2c0.ifBusy = false;
}else if(i2c_v == I2C1) {
    NVIC_EnableIRQ(I2C1_IRQn);
    i2c1.ifBusy = false;
}

i2cx_bus_reset(i2c_v);



}


/***************************************************************************/
/**
 * @brief
 *   I2C bus start function
 *
 * @details
 *  This function is used to start the interrupt driven state machine. It first assigns all the necessary values to run the state machine such as
 *  bytesLeft, bufferAddres
 *
 * @note
 *   This function is used to reset the I2C bus and to
 *
 * @param[in] i2c pointer
 *   Pointer to the base peripheral address of the i2c peripheral being used
 *
 *
 ******************************************************************************/
void i2c_start(I2C_TypeDef *i2c, STATE_MACHINE_START_STRUCT *openStruct){

  I2C_STATE_MACHINE *i2cx_state_machine;


  if(i2c == I2C0){
      i2cx_state_machine = &i2c0;
  }
  else if(i2c == I2C1){
      i2cx_state_machine = &i2c1;
  }

  while(i2cx_state_machine->ifBusy == true){
  }

  sleep_block_mode(I2C_EM_BLOCK);
  i2cx_state_machine->ifBusy = true;
  i2cx_state_machine->I2Cx = i2c;

  i2cx_state_machine->bufferAddress = openStruct->newBufferAddress;
  i2cx_state_machine->byteLefts = openStruct->newBytesleft;
  i2cx_state_machine->current_state = Init;
  i2cx_state_machine->deviceAddress = openStruct->newDeviceAddress;
  i2cx_state_machine->read = openStruct->newRead;
  i2cx_state_machine->I2C_CallBackEvent = openStruct->newCallBack;
  i2cx_state_machine->command = openStruct->newCommand;
  i2cx_state_machine->combinedBytes = openStruct->newCombinedBytes;
  i2cx_state_machine->numCmdBytes = openStruct->newNumCmdBytes;
  i2cx_state_machine->data = openStruct->newData;



  i2cx_state_machine->current_state = Init;

  i2c->CMD = I2C_CMD_START;
  i2c->TXDATA = ((WdeviceAdd << 1) | WRITE);



}

/***************************************************************************/
/**
 * @brief
 *  ACK interrupt driver function
 *
 *
 * @details
 *   This function handles the functionality of the interrupt driven state machine when an ACK interrupt is received.
 *
 * @note
 *   This function uses switch cases to handle the different states that the EFM-32 can receive ACKS in.
 *
 * @param[in] i2c pointer
 *   Pointer to the i2c peripheral being used.
 *
 *
 ******************************************************************************/
void ACK_Interrupt(I2C_STATE_MACHINE *i2c_sm){
//  if(i2c_sm->current_state == SetReg){
//      i2c_sm->I2Cx->TXDATA = i2c_sm->command;
//  }
//  if(i2c_sm->current_state == Hold){
//      i2c_sm->I2Cx->TXDATA = i2c_sm->I2Cx->;
//  }
  switch(i2c_sm->current_state){
    case Init:

      i2c_sm->numCmdBytes--;
      i2c_sm->I2Cx->TXDATA = (i2c_sm->command) >> (8*(i2c_sm->numCmdBytes));
        if(i2c_sm->numCmdBytes == 0){
         if(i2c_sm->read == false){
             i2c_sm->current_state = Write;
         }
         else{
             i2c_sm->current_state = SetReg;
         }
        }
      break;
    case SetReg:
      if(i2c_sm->read == 1){
          if(i2c_sm->deviceAddress == SH_address){
              i2c_sm->I2Cx->CMD = I2C_CMD_STOP;
          }
          i2c_sm->I2Cx->CMD = I2C_CMD_START;
          i2c_sm->I2Cx->TXDATA = i2c_sm->deviceAddress << 1 | READ;
          i2c_sm->current_state = Hold;
      }
      else{
          EFM_ASSERT(false);
      }

      break;
    case Hold:
      i2c_sm->current_state = RXState;
//      i2c_sm->bufferAddress = bufferAddressReset;

      break;
    case Write:
//          if(i2c_sm->byteLefts >= 1){
//
//              i2c_sm->I2Cx->TXDATA = (*i2c_sm->bufferAddress >> (8 * (i2c_sm->byteLefts - 1)));
//              i2c_sm->byteLefts --;
//
//          }
//          else{
               i2c_sm->I2Cx->CMD = I2C_CMD_STOP;
               i2c_sm->current_state = Close;
//          }
      break;
    case RXState:
      EFM_ASSERT(false);
      break;
    case Close:
      EFM_ASSERT(false);
      break;
    default:
      EFM_ASSERT(false);
      break;

  }


}



/***************************************************************************/
/**
 * @brief
 * NACK interrupt driver function
 *
 *
 * @details
 *  This function handles the functionality of the state machine when a NACK Interrupt is received by the EFM-32 from the Sensor.
 * @note
 *   This function uses switch cases to handle the different tasks of the state machine depending on the state of the state machine
 *
 * @param[in] i2c pointer
 *   Pointer to the i2c peripheral being used.
 *
 *
 ******************************************************************************/

void NACK_Interrupt(I2C_STATE_MACHINE *i2c_sm){
  switch(i2c_sm->current_state){
    case Init:
      i2c_sm->I2Cx->CMD = I2C_CMD_START;
      i2c_sm->I2Cx->TXDATA = (i2c_sm->deviceAddress << 1 | WRITE);
      i2c_sm->numCmdBytes = (i2c_sm->combinedBytes - i2c_sm->byteLefts);
      break;
    case SetReg:
      i2c_sm->I2Cx->TXDATA = i2c_sm->command;
      break;
    case Hold:
      if(i2c_sm->read == 1){
          if(i2c_sm->deviceAddress == SH_address){
              i2c_sm->I2Cx->CMD  = I2C_CMD_STOP;
          }
          i2c_sm->I2Cx->CMD = I2C_CMD_START;
          i2c_sm->I2Cx->TXDATA = i2c_sm->deviceAddress << 1 | READ;
      }
      else{
          break;
      }
      break;
    case RXState:
      EFM_ASSERT(false);
      break;
    case Close:
      EFM_ASSERT(false);
      break;
    default:
      EFM_ASSERT(false);
      break;
  }

}

/***************************************************************************/
/**
 * @brief
 *   RXDATAV Interrupt driver
 *
 * @details
 *  This function handles the functionality of the state machine when a RXDATAV Interrupt is received by the EFM-32 from the Sensor.
 * @note
 *   This function uses switch cases to handle the different tasks of the state machine depending on the state of the state machine
 *
 * @param[in] i2c pointer
 *   Pointer to the i2c peripheral being used.
 *
 *
 ******************************************************************************/

void RXDATAV_Interrupt(I2C_STATE_MACHINE *i2c_sm){

    switch(i2c_sm->current_state){
      case Init:
              EFM_ASSERT(false);
            break;
          case SetReg:
              EFM_ASSERT(false);
            break;
          case Hold:
            EFM_ASSERT(false);
            break;

          case RXState:
            if(i2c_sm->read == 1){
                *(i2c_sm->bufferAddress) = ((i2c_sm->I2Cx->RXDATA) | *i2c_sm->bufferAddress << 8);
                    if(i2c_sm->byteLefts >= 2){
                        i2c_sm->I2Cx->CMD = I2C_CMD_ACK;
                        i2c_sm->byteLefts --;

                    }
                    else{
                         i2c_sm->I2Cx->CMD = I2C_CMD_NACK;
                         i2c_sm->I2Cx->CMD = I2C_CMD_STOP;
                         i2c_sm->current_state = Close;
                    }

            }
            else{
                break;
            }


            break;

          case Close:
            EFM_ASSERT(false);
            break;
          default:
            EFM_ASSERT(false);
            break;
    }

}

/***************************************************************************/
/**
 * @brief
 *   MSTOP Interrupt driver
 *
 * @details
 *  This function handles the functionality of the state machine when a MSTOP Interrupt is received by the EFM-32 from the Sensor.
 * @note
 *   This function uses switch cases to handle the different tasks of the state machine depending on the state of the state machine
 *
 * @param[in] i2c pointer
 *   Pointer to the i2c peripheral being used.
 *
 *
 ******************************************************************************/

void MSTOP_Interrupt(I2C_STATE_MACHINE *i2c_sm){
  switch(i2c_sm->current_state){
        case Init:
          EFM_ASSERT(false);
          break;
        case SetReg:
          EFM_ASSERT(false);
          break;
        case Hold:
          EFM_ASSERT(false);
          break;
        case RXState:
          EFM_ASSERT(false);

          break;
        case Close:
          if(i2c_sm->read == 1){
              add_scheduled_events(i2c_sm->I2C_CallBackEvent);
              sleep_unblock_mode(I2C_EM_BLOCK);
              i2c_sm->ifBusy = false;
          }
          if(i2c_sm->read == 0){
              sleep_unblock_mode(I2C_EM_BLOCK);
              i2c_sm->ifBusy = false;
          }
          break;
        default:
          EFM_ASSERT(false);
          break;
  }
}


/***************************************************************************/
/**
 * @brief
 * I2C0 Interrupt Handler function
 *
 * @details
 * This function determines the interrupt flags that are both raised and enabled, and then calls the corresponding Interrupt driver in order to progress through the
 * state machine
 *
 *
 * @param[in] void
 *
 *
 ******************************************************************************/

void I2C0_IRQHandler(void){

  int int_flag;
  int_flag = (I2C0->IF & I2C0->IEN);
  I2C0->IFC = int_flag;

if(int_flag & I2C_IF_ACK){
    ACK_Interrupt(&i2c0);
}
if(int_flag & I2C_IF_NACK){
    NACK_Interrupt(&i2c0);
}
if(int_flag & I2C_IF_RXDATAV){
    RXDATAV_Interrupt(&i2c0);
}
if(int_flag & I2C_IF_MSTOP){
    MSTOP_Interrupt(&i2c0);
    }

}

/***************************************************************************/
/**
 * @brief
 * I2C1 Interrupt Handler function
 *
 * @details
 * This function determines the interrupt flags that are both raised and enabled, and then calls the corresponding Interrupt driver in order to progress through the
 * state machine
 *
 *
 * @param[in] void
 *
 *
 ******************************************************************************/
void I2C1_IRQHandler(void){

  int int_flag;
  int_flag = (I2C1->IF & I2C1->IEN);
  I2C1->IFC = int_flag;

if(int_flag == I2C_IF_ACK){
    ACK_Interrupt(&i2c1);
}
if(int_flag == I2C_IF_NACK){
    NACK_Interrupt(&i2c1);
}
if(int_flag == I2C_IF_RXDATAV){
    RXDATAV_Interrupt(&i2c1);
}
if(int_flag == I2C_IF_MSTOP){
    MSTOP_Interrupt(&i2c1);
    }
}






