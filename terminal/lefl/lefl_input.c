/*
 * lefl_input.c
 */
#include "lefl.h"

void lefl_key_update(lefl_key_t* key,bool state)
{
    if((!(key->state))&&state)
    {
        key->trigger=true;
        if(key->key_cb!=NULL)
            key->key_cb(key);
    }
    if(!state)
    {
        key->trigger=false;
    }
    key->state=state;
}
bool lefl_key_is_triggered(lefl_key_t* key)
{
    if(key->trigger)
    {
        key->trigger=false;
        return true;
    }
    else
    {
        return false;
    }
}

void lefl_advanced_key_update(lefl_advanced_key_t* key,float value)
{
    switch(key->mode)
    {
    case LEFL_KEY_DIGITAL_MODE:
        if((!(key->state))&&value)
        {
            if(key->state==false&&key->key_cb!=NULL)
                key->key_cb(key);
            key->trigger=true;
        }
        if(!value)
        {
            key->trigger=false;
        }
        key->state=value;
        break;
    case LEFL_KEY_ANALOG_NORMAL_MODE:
        key->value=value;
        if(key->value-key->schmitt_parameter>key->trigger_distance)
        {
            if(key->state==false&&key->key_cb!=NULL)
                key->key_cb(key);
            key->state=true;
        }
        if(key->value+key->schmitt_parameter<key->trigger_distance)
        {
            key->state=false;
        }
        break;
    case LEFL_KEY_ANALOG_RAPID_MODE:
        key->value=value;
        if (key->state)
        {
            if (key->value - key->minimum - key->schmitt_parameter >= key->trigger_distance)
            {
                if (key->value > key->maximum)
                    key->maximum = key->value;
            }
            if (key->maximum - key->value - key->schmitt_parameter >= key->release_distance)
            {
                key->minimum = key->value;
                key->state = false;
            }
        }
        else
        {
            if (key->value - key->minimum - key->schmitt_parameter >= key->trigger_distance)
            {
                key->maximum = key->value;
                if(key->state==false&&key->key_cb!=NULL)
                    key->key_cb(key);
                key->state = true;
            }
            if (key->maximum - key->value - key->schmitt_parameter >= key->release_distance)
            {
                if (key->value < key->minimum)
                    key->minimum = key->value;
            }
        }
        if (key->value <= 0.0)
        {
            key->state = false;
            key->minimum = 0.0;
        }
        if (key->value >= 1.0)
        {
            if(key->state==false&&key->key_cb!=NULL)
                key->key_cb(key);
            key->state = true;
            key->maximum = 1.0;
        }
        break;
    case LEFL_KEY_ANALOG_SPEED_MODE:
        if(value-key->value > key->trigger_speed)
        {
            if(key->state==false&&key->key_cb!=NULL)
                key->key_cb(key);
            key->state=true;
        }
        if(value-key->value < key->release_speed)
        {
            key->state=false;
        }
        key->value=value;
        break;
    }
}

void lefl_advanced_key_update_raw(lefl_advanced_key_t* key, int16_t value)
{
    key->raw=value;
    if(key->mode==LEFL_KEY_DIGITAL_MODE)
        lefl_advanced_key_update(key,value);
    else
        lefl_advanced_key_update(key,lefl_advanced_key_normalize(key, value));
}

void lefl_advanced_key_update_state(lefl_advanced_key_t* key, bool state)
{
    if((!(key->state))&&state)
    {
        key->trigger=true;
    }
    if(!state)
    {
        key->trigger=false;
    }
    key->state=state;
}

float lefl_advanced_key_normalize(lefl_advanced_key_t* key, int16_t value)
{
    float x;
    x=(key->upper_bound-(key->upper_bound - key->lower_bound)*key->upper_deadzone -value)/(key->range);
    return x;
}

bool lefl_advanced_key_is_triggered(lefl_advanced_key_t* key)
{
    if(key->trigger)
    {
        key->trigger=false;
        return true;
    }
    else
    {
        return false;
    }
}

void lefl_advanced_key_set_range(lefl_advanced_key_t* key,float upper,float lower)
{
    key->upper_bound=upper;
    key->lower_bound=lower;
    key->range = key->upper_bound - key->lower_bound;
}

void lefl_advanced_key_set_deadzone(lefl_advanced_key_t* key,float upper,float lower)
{
    /*
    key->upper_deadzone = (key->upper_bound - key->lower_bound)*upper;
    key->lower_deadzone = (key->upper_bound - key->lower_bound)*lower;
    key->range = (key->upper_bound - key->upper_deadzone) - (key->lower_bound+key->lower_deadzone);
    */
    key->upper_deadzone = upper;
    key->lower_deadzone = lower;
    key->range = (key->upper_bound - (key->upper_bound - key->lower_bound)*key->upper_deadzone) - (key->lower_bound+(key->upper_bound - key->lower_bound)*key->lower_deadzone);
}
