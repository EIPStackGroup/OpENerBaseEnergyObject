/*******************************************************************************
 * Copyright (c) 2019, Rockwell Automation, Inc.
 * All rights reserved.
 *
 ******************************************************************************/

#ifndef CIP_BASE_ENERGY_OBJECT_H_
#define CIP_BASE_ENERGY_OBJECT_H_

#include "opener_api.h"
#include "typedefs.h"

static const CipUint kCipBaseEnergyClassCode = 0x4e; /**< Base Energy Object class code */

/* base energy public types and definitions */

EipStatus
CipBaseEnergyInit(void);

void UpdateOdometers(CipInt energy_change_in_wh);

CipLint total_energy_value;
CipUlint consumed_energy_value;
CipUlint produced_energy_value;

EipStatus
GetAttributeSingleBaseEnergy(CipInstance *instance,
                             CipMessageRouterRequest *message_router_request,
                             CipMessageRouterResponse *message_router_response,
                             const struct sockaddr *originator_address,
                             const int encapsulation_session);

EipStatus
GetAttributeAllBaseEnergy(CipInstance *instance,
                          CipMessageRouterRequest *message_router_request,
                          CipMessageRouterResponse *message_router_response,
                          const struct sockaddr *originator_address,
                          const int encapsulation_session);

EipStatus SetAttributeSingleBaseEnergy(CipInstance *instance,
                                       CipMessageRouterRequest *message_router_request,
                                       CipMessageRouterResponse *message_router_response,
                                       const struct sockaddr *originator_address,
                                       const int encapsulation_session);

/********************* public functions **************************************/
/* \defgroup CIP_ENERGY API of OpENer's CIP Energy implementation
 *
 */

/* \ingroup CIP_ENERGY_API
 * \brief function that will handle the application processing of the base energy object
 *
 * Will be invoked right before the void IApp_HandleApplication(void) callback.
 */
void HandleBaseEnergyObject(
  );

/**  \defgroup CIP_ENERGY_API Callback Functions Demanded by CIP Energy
 * \ingroup CIP_ENERGY_API
 *
 * \brief These functions have to implemented in order to give the CIP Energy
 * implementation of OpENer a method to inform the application on certain state changes.
 */

/** \ingroup CIP_ENERGY_CALLBACK_API
 * \brief Callback informing that the cip energy object is entering the state
 * Metering
 */
void
BaseEnergyOnEnterStateMetering(
  );

/** \ingroup CIP_ENERGY_CALLBACK_API
 * \brief Callback informing that the cip energy object is entering the state
 * NotMetering
 */
void
BaseEnergyOnEnterStateNotMetering(
  );

/** \ingroup CIP_ENERGY_CALLBACK_API
 * \brief Callback informing that the cip energy object is entering the state
 * Starting
 */
void
BaseEnergyOnEnterStateStarting(
  );

/** \ingroup CIP_ENERGY_CALLBACK_API
 * \brief Callback informing that the cip energy object is entering the state
 * Stopping
 */
void
BaseEnergyOnEnterStateStopping(
  );

/** \ingroup CIP_ENERGY_CALLBACK_API
 * \brief Application specific processing during the energy object state
 * Metering.
 *
 * State processing is triggered cyclically synchronously with the manage
 * connections cycle.
 *
 * Transitional state: Application informs with true return value that the process
 * in this state has finished.
 */
CipBool
BaseEnergyProcessStateMetering(
  );

/** \ingroup CIP_ENERGY_CALLBACK_API
 * \brief Application specific processing during the energy object state
 * NotMetering.
 *
 * State processing is triggered cyclically synchronously with the manage
 * connections cycle.
 *
 * Transitional state: Application informs with true return value that the process
 * in this state has finished.
 */
CipBool
BaseEnergyProcessStateNotMetering(
  );

/** \ingroup CIP_ENERGY_CALLBACK_API
 * \brief Application specific processing during the energy object state
 * Starting.
 *
 * State processing is triggered cyclically synchronously with the manage
 * connections cycle.
 *
 * Transitional state: Application informs with true return value that the process
 * in this state has finished.
 */
CipBool
BaseEnergyProcessStateStarting(
  );

/** \ingroup CIP_ENERGY_CALLBACK_API
 * \brief Application specific processing during the energy object state
 * Stopping.
 *
 * State processing is triggered cyclically synchronously with the manage
 * connections cycle.
 *
 * Transitional state: Application informs with true return value that the process
 * in this state has finished.
 */
CipBool
BaseEnergyProcessStateStopping(
  );

int encodeUINTOdometer(CipUlint odometer_value,
                       CipOctet **message);
int encodeINTOdometer(CipLint odometer_value,
                      CipOctet **message);
#endif /* IP_BASE_ENERGY_OBJECT_H_ */
