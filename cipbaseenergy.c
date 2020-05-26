/*******************************************************************************
 * Copyright (c) 2019, Rockwell Automation, Inc.
 * All rights reserved.
 *
 ******************************************************************************/
#include "cipbaseenergy.h"
#include "opener_api.h"
#include "trace.h"
#include "endianconv.h"
#include <string.h>
#include <cipcommon.h>
#include <math.h>

#define ODOMETER_POSITIONS 5u

/* Base Energy Object, Class Code 0x4E according to CIP-Library Vol. 1 */

CipUint energy_resource_type = 0; /**< #1 Specifies the Energy/Resource Type */
CipUint base_energy_object_capabilities = 0; /**< #2 Specifies the Base Energy Object Capabilities */
CipUint energy_accuracy = 0; /**< #3 Specifies the Energy Accuracy*/
CipUint energy_accuracy_basis = 0; /**< #4 Specifies the Energy Accuracy Basis */
CipReal full_scale_reading = 0.0; /**< #5 Specifies the Full Scale Energy Transfer Rate */
CipUint data_status = 0; /**< #6 Specifies the Status of the Instance or Aggregation Data */
CipReal energy_transfer_rate = 0.0; /**< #10 Specifies the time rate of energy consumption or production */
CipReal energy_transfer_rate_user_setting = 0.0; /**< #11 Specifies the User setting for fixed, derived or proxy power value */
CipEpath energy_type_specific_object_path = { 3, 0x4f, 1, 0 }; /**< #12 Path to Energy Type Specific Object instance */
CipUint energy_aggregation_path_array_size = 0; /**< #13 Specifies the Number Members in the "Energy Aggregation Paths" Array */

CipLint total_energy_value = 0; /**< #9 Specifies the total net energy value */
CipUlint consumed_energy_value = 0; /**< #7 Specifies the consumed energy value */
CipUlint produced_energy_value = 0; /**< #8 Specifies the generated energy value */

/** services **/

void InitializeCipBaseEnergy(CipClass *class) {

  CipClass *meta_class = class->class_instance.cip_class;

  InsertAttribute( (CipInstance *) class, 1, kCipUint, EncodeCipUint,
                   (void *) &class->revision,
                   kGetableSingleAndAll ); /* revision */

  InsertService(meta_class, kGetAttributeSingle,
                &GetAttributeSingle, "GetAttributeSingle");

}

/** class attribute data **/

EipStatus CipBaseEnergyInit(void) {
  CipClass *base_energy_class = NULL;
  CipInstance *instance;

  base_energy_class = CreateCipClass(kCipBaseEnergyClassCode, /* class code */
                                     1, /* # of class attributes*/
                                     1, /* # highest class attribute number*/
                                     1, /* # class services*/
                                     13, /* # instance attributes*/
                                     13, /* # highest instance attribute number*/
                                     3, /* # instance services*/
                                     1, /* # instances*/
                                     "Base Energy", /* class name */
                                     2, /* # class revision*/
                                     &InitializeCipBaseEnergy); /* # function pointer for initialization*/

  if (NULL == base_energy_class) {
    return kEipStatusError;
  }

  instance = GetCipInstance(base_energy_class, 1);
  InsertAttribute(instance, 1, kCipUint, EncodeCipUint, &energy_resource_type,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 2, kCipUint,EncodeCipUint, &base_energy_object_capabilities,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 3, kCipUint,EncodeCipUint, &energy_accuracy,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 4, kCipUint,EncodeCipUint, &energy_accuracy_basis,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 5, kCipReal, EncodeCipReal, &full_scale_reading, kSetAndGetAble);
  InsertAttribute(instance, 6, kCipUint, EncodeCipUint, &data_status, kGetableSingleAndAll);
  InsertAttribute(instance, 7, kCipAny, EncodeUINTOdometer, &consumed_energy_value,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 8, kCipAny, EncodeUINTOdometer, &produced_energy_value,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 9, kCipAny, EncodeINTOdometer, &total_energy_value,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 10, kCipReal, EncodeCipReal, &energy_transfer_rate,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 11, kCipReal, EncodeCipReal, &energy_transfer_rate_user_setting,
                  kSetAndGetAble);
  InsertAttribute(instance, 12, kCipEpath, EncodeCipEPath, &energy_type_specific_object_path,
                  kGetableSingleAndAll);
  InsertAttribute(instance, 13, kCipUint, EncodeCipUint, &energy_aggregation_path_array_size,
                  kGetableSingleAndAll);

  /*Required Instance Attributes: Energy Type Specific Object Path (ID=12): Struct of:
   *  Path Size (UINT)
   *  Path (EPATH) */

  /*none implemented because no class attributes are implemented so far
   * use Class-specific GetAttributeSingle method to allow special handling of odo-meter-values
   */
  InsertService(base_energy_class, kGetAttributeSingle,
                &GetAttributeSingle, "GetAttributeSingle");

  InsertService(base_energy_class, kGetAttributeAll, &GetAttributeAll,
                "GetAttributeAll");

  InsertService(base_energy_class, kSetAttributeSingle,
                &SetAttributeSingleBaseEnergy, "SetAttributeSingle");

/* TODO: open services: implement reset-service; implement state-change (stop/start metering); */

  /* startup the measurement and metering functionality */
//  enterStateInitializing();
  return kEipStatusOk;
}

/*****************************************************************************
 *
 * provide encoding of Array[5] of UINT as used by produced energy and
 * consumed energy odometers
 *
 ******************************************************************************/
void EncodeUINTOdometer(const void *const data, ENIPMessage *const outgoing_message) {
  CipUint odometer[ODOMETER_POSITIONS] = { 0, 0, 0, 0, 0 };  /* Wh, kWh, MWh, GWh, TWh */
  CipUlint odometer_value = *(const CipUlint *const) data;

  for (size_t i = 0; i < ODOMETER_POSITIONS; i++) {
    odometer[i] = odometer_value % 1000;
    EncodeCipUint(&(odometer[i]), outgoing_message);  /* Wh, kWh, MWh, GWh, TWh */
    odometer_value /= 1000;
  }

}

/*****************************************************************************
 *
 * provide encoding of Array[5] of INT as used by net energy odometer
 *
 ******************************************************************************/
void EncodeINTOdometer(const void *const data, ENIPMessage *const outgoing_message) {
  CipInt odometer[ODOMETER_POSITIONS] = { 0, 0, 0, 0, 0 };  /* Wh, kWh, MWh, GWh, TWh */
  CipLint odometer_value = *(const CipLint *const) data;

  for (size_t i = 0; i < ODOMETER_POSITIONS; i++) {
    odometer[i] = odometer_value % 1000;
    EncodeCipInt(&(odometer[i]), outgoing_message);  /* Wh, kWh, MWh, GWh, TWh */
  }
}

EipStatus SetAttributeSingleBaseEnergy(
  CipInstance *const RESTRICT instance,
  CipMessageRouterRequest *const RESTRICT message_router_request,
  CipMessageRouterResponse *const RESTRICT message_router_response,
  const struct sockaddr *originator_address,
  const int encapsulation_session) {

  EipStatus return_value = kEipStatusOkSend;
  int nDecodeStatus = 0;
  const CipUsint *paMsg = message_router_request->data;

  message_router_response->reply_service = (0x80
                                            | message_router_request->service);
  message_router_response->general_status = kCipErrorAttributeNotSupported;
  message_router_response->size_of_additional_status = 0;
  InitializeENIPMessage(&message_router_response->message);

  CipAttributeStruct *attribute = GetCipAttribute(
    instance, message_router_request->request_path.attribute_number);
  (void) instance; /*Suppress compiler warning */

  message_router_response->general_status = kCipErrorSuccess;

  uint8_t set_bit_mask = (instance->cip_class->set_bit_mask[CalculateIndex(
                                                              message_router_request
                                                              ->request_path.
                                                              attribute_number)]);
  if (NULL != attribute
      && message_router_request->request_path.attribute_number
      <= instance->cip_class->highest_attribute_number
      && message_router_request->request_path.attribute_number > 0) {
    if (set_bit_mask
        & (1 <<
           ( (message_router_request->request_path.attribute_number) % 8 ) ) ) {

      switch (message_router_request->request_path.attribute_number) {
        case 5: {
          if (-1
              != (nDecodeStatus = DecodeData(kCipReal, &full_scale_reading,
                                             &paMsg) ) ) {
            OPENER_ASSERT(true);
          }
        }
        break;

        case 11: /* max_offsetFromMaster */
        {
          if (-1
              != (nDecodeStatus = DecodeData(kCipReal,
                                             &energy_transfer_rate_user_setting,
                                             &paMsg) ) ) {
            OPENER_ASSERT(true);
          }
        }
        break;
      }
    } else{
      message_router_response->general_status = kCipErrorAttributeNotSetable;
    }
  } else{
    message_router_response->general_status = kCipErrorAttributeNotSupported;
  }
  return return_value;
}

void UpdateOdometers(CipInt energy_change_in_wh) {

  if (0 <= energy_change_in_wh) {  //pos Values - consumed energy
    consumed_energy_value += energy_change_in_wh;  //TODO: add overflow-protection
    total_energy_value += energy_change_in_wh;  //TODO: add overflow-protection
  } else {
    produced_energy_value -= energy_change_in_wh;  //TODO: add overflow-protection; subtract because of negative sign
    total_energy_value += energy_change_in_wh;  //TODO: add overflow-protection; add because of negative sign of EnergyChange
  }

  CipLint max_odometer_value = 999999999999999L;
  CipLint min_odometer_value = -999999999999999L;

  if (max_odometer_value < consumed_energy_value) {
    consumed_energy_value -= max_odometer_value;
  }
  if (max_odometer_value < produced_energy_value) {
    produced_energy_value -= max_odometer_value;
  }
  if (max_odometer_value < total_energy_value) {
    total_energy_value -= max_odometer_value;
  }
  if (min_odometer_value > total_energy_value) {
    total_energy_value -= min_odometer_value;
  }

}

