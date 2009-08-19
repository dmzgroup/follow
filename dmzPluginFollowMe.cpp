#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
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
      TimeSlice (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      EventObserverUtil (Info, local),
      _log (Info),
      _isect (0),
      _hil (0),
      _me (0),
      _defaultAttrHandle (0),
      _hilAttrHandle (0),
      _targetAttrHandle (0),
      _minDistance (15.0),
      _speed (24.0) {

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


// TimeSlice Interface
void
dmz::PluginFollowMe::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod = get_object_module ();

   if (objMod && _me && !is_zero64 (TimeDelta)) {

      Mask state;

      objMod->lookup_state (_me, _defaultAttrHandle, state);

      if (state.contains (_deadState)) {

         objMod->store_velocity (_me, _defaultAttrHandle, Vector (0.0, 0.0, 0.0));
      }
      else {

         Vector position;
         Vector positionNew;
         Matrix orientation;

         objMod->lookup_position (_me, _defaultAttrHandle, position);
         objMod->lookup_orientation (_me, _defaultAttrHandle, orientation);

         _move (TimeDelta, position, orientation, positionNew);
         _turn_and_clamp (positionNew, positionNew, orientation);

         Vector velocity = (position - positionNew) * (1.0 /  TimeDelta);

         objMod->store_position (_me, _defaultAttrHandle, positionNew);
         objMod->store_orientation (_me, _defaultAttrHandle, orientation);
         objMod->store_velocity (_me, _defaultAttrHandle, velocity);
      }
   }
}


// Input Observer Interface
void
dmz::PluginFollowMe::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

   if (_me && ((Value.get_key () == 'u') && Value.get_key_state ())) {

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         Mask state;
         objMod->lookup_state (_me, _defaultAttrHandle, state);
         state.unset (_deadState | _smokeAndFireState);
         objMod->store_state (_me, _defaultAttrHandle, state);
      }
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


// Event Observer Interface
void
dmz::PluginFollowMe::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   EventModule *eventMod (get_event_module ());

   if (_me && eventMod && Type.is_of_type (_detonationType)) {

      Handle target (0);

      if (eventMod->lookup_object_handle (EventHandle, _targetAttrHandle, target)) {

         if (target == _me) {

            ObjectModule *objMod (get_object_module ());

            if (objMod) {

               Mask state;
               objMod->lookup_state (_me, _defaultAttrHandle, state);
               state |= _deadState | _smokeAndFireState;
               objMod->store_state (_me, _defaultAttrHandle, state);
            }
         }
      }
   }
}


// PluginFollowMe Interface
void
dmz::PluginFollowMe::_move (
      const Float64 TimeDelta,
      const Vector Position,
      const Matrix Orientation,
      Vector &positionNew) {

   const Float64 CurrentDistance = (_targetPosition - Position).magnitude ();

   if (CurrentDistance > _minDistance) {

      Vector dir (0.0, 0.0, -1.0);
      Orientation.transform_vector (dir);

      positionNew = Position + (dir * (TimeDelta * _speed));
   }
   else { positionNew = Position; }
}


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

   RuntimeContext *context = get_plugin_runtime_context ();

   activate_default_input_channel (InputEventKeyMask);

   _defaultAttrHandle = activate_default_object_attribute (ObjectPositionMask);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   _targetAttrHandle = config_to_named_handle (
      "attribute.target.name",
      local,
      EventAttributeTargetName,
      context);

   _detonationType = activate_event_callback (EventDetonationName, EventCloseMask);

   _deadState = config_to_state (
      "state.dead.name",
      local,
      DefaultStateNameDead,
      context);

   const Mask SmokingState = config_to_state (
      "state.smoking.name",
      local,
      DefaultStateNameSmoking,
      context);

   const Mask FireState = config_to_state (
      "state.fire.name",
      local,
      DefaultStateNameFire,
      context);

   _smokeAndFireState = SmokingState | FireState;
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
