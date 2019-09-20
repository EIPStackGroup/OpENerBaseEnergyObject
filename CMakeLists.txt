#################################
# Base Energy object class 0x4E #
#################################
opener_add_cip_object( BaseEnergyObject "Base Energy Object (class 0x4E)" )
#################################
# Add common includes           #
#################################
opener_common_includes()

##################################
# Add platform-specific includes #
##################################
opener_platform_support("INCLUDES")


#################################
# Set source for library        #
#################################
set( BASE_ENERGY_OBJECT_SOURCE cipbaseenergy.c )

#################################
# Create library		#
#################################
add_library( BASE_ENERGY_OBJECT ${BASE_ENERGY_OBJECT_SOURCE} )

string(STRIP "${OpENer_ADD_CIP_OBJECTS} BASE_ENERGY_OBJECT" OpENer_TRIMMED_ADD_CIP_OBJECTS)

set(OpENer_ADD_CIP_OBJECTS "${OpENer_TRIMMED_ADD_CIP_OBJECTS}" CACHE INTERNAL STRING )
target_link_libraries( BASE_ENERGY_OBJECT CIP)


