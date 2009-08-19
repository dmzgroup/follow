#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include "dmzPluginFollowMe.h"
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToState.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRenderIsectUtil.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>

dmz::PluginFollowMe::PluginFollowMe (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _isect (0),
      _hil (0),
      _me (0),
      _defaultAttrHandle (0),
      _hilAttrHandle (0),
      _minDistance (15.0) {

   _init (local);
}


dmz::PluginFollowMe::~PluginFollowMe () {

}


// Plugin Interface
void
dmz::PluginFollowMe::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      if (_me) {

         ObjectModule *objMod = get_object_module ();

         if (objMod) { objMod->destroy_object (_me); _me = 0; }
      }
   }
}


void
dmz::PluginFollowMe::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_isect) { _isect = RenderModuleIsect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_isect && (_isect == RenderModuleIsect::cast (PluginPtr))) { _isect = 0; }
   }
}


// Object Observer Interface
void
dmz::PluginFollowMe::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _hilAttrHandle) {

      if (Value) {

         _hil = ObjectHandle;

         ObjectModule *objMod = get_object_module ();

         if (objMod) {

            _me = objMod->create_object (objMod->lookup_object_type (_hil), ObjectLocal);

            objMod->lookup_position (_hil, _defaultAttrHandle, _targetPosition);

            Vector offset (0.0, 0.0, -(_minDistance + 1.0));

            Matrix orientation;
            objMod->lookup_orientation (_hil, _defaultAttrHandle, orientation);
            orientation.transform_vector (offset);

            Vector position (_targetPosition + offset);

            _turn_and_clamp (position, position, orientation);

            objMod->store_position (_me, _defaultAttrHandle, position);
            objMod->store_orientation (_me, _defaultAttrHandle, orientation);

            objMod->activate_object (_me);
         }
      }
      else if (ObjectHandle == _hil) {

         _hil = 0;
      }
   }
}


void
dmz::PluginFollowMe::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (ObjectHandle == _hil) { _targetPosition = Value; }
}


// PluginFollowMe Interface
void
dmz::PluginFollowMe::_turn_and_clamp (
      const Vector Position,
      Vector &positionNew,
      Matrix &orientationNew) {

   if (_isect && _me) {

      Vector normal (0.0, 1.0, 0.0);
      isect_clamp_point (Position, *_isect, positionNew, normal);

      Vector headingVec (_targetPosition - Position);
      headingVec.set_y (0.0);
      headingVec.normalize_in_place ();

      Matrix heading;

      if (headingVec.is_zero () == False) {

         heading.from_two_vectors (Vector (0.0, 0.0, -1.0), headingVec);
      }

      Matrix pitchAndRoll (Vector (0.0, 1.0, 0.0), normal);

      orientationNew = pitchAndRoll * heading;
   }
   else { positionNew = Position; }
}


void
dmz::PluginFollowMe::_init (Config &local) {

   _defaultAttrHandle = activate_default_object_attribute (ObjectPositionMask);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginFollowMe (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginFollowMe (Info, local);
}

};
