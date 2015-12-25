////////////////////////////////////////////////////////////////////////////////

#include <map>
#include "gwRTTI.h"

namespace gw
{
    namespace RTTI
    {
        //------------------------------------------------------------------------------
        // util functions
        //------------------------------------------------------------------------------
        
        static uint32_t DJB2( const char* str )
        {
            uint32_t hash = 5381;
            
            if( str )
            {
                while( uint32_t c = (uint32_t) ( *str++ ) )
                {
                    hash = ((hash << 5) + hash) + c; // hash * 33 + c
                }
            }
            
            return hash;
        }
        
        
        //------------------------------------------------------------------------------
        // Registry
        //------------------------------------------------------------------------------

        const TypeInfo* Registry::Find( const char* name )
        {
            auto itr = mTypesByName.find( DJB2( name ) );
            return itr != mTypesByName.end() ? itr->second : nullptr;
        }
        
        const TypeInfo* Registry::FindOrCreate( const char* funcsig )
        {
            return &mTypes[ DJB2( funcsig ) ];
        }
        
        void Registry::Register( TypeInfo* info )
        {
            info->Hash          = DJB2( info->Name );
            info->IsRegistered  = true;
            
            mTypesByName[ info->Hash ] = info;
        }
        

        
        //------------------------------------------------------------------------------
        // TypeInfo
        //------------------------------------------------------------------------------
        
        bool TypeInfo::IsA( const TypeInfo* type ) const
        {
            const TypeInfo** info = BaseClasses;
            
            do
            {
                if( *info == type )
                {
                    return true;
                }
                
                ++info;
            }
            while( *info );
            
            return false;
        }
        
        
        //------------------------------------------------------------------------------
        // standard value converters
        //------------------------------------------------------------------------------
        
        
        // value types to strings
        
        static const char* EnumTypeToString( const TypeInfo* type, void* obj, char* buffer, int size )
        {
            // look up value
            
            int value = *((int*)obj);
            
            if( type->Constants )
            {
                auto c = type->Constants;
                auto i = type->NumMembers;
                
                while( i-- )
                {
                    if( c->Value == value )
                    {
                        return c->Name;
                    }
                    
                    ++c;
                }
            }
            
            sprintf( buffer, "%i", value );
            return buffer;
        }
        
        static const char* FundamentalTypeToString( const TypeInfo* type, void* obj, char* buffer, int size )
        {
            switch( type->Hash )
            {
                case 0x7c94b391: // bool
                    sprintf( buffer, "%s", *((bool*)obj) ? "true" : "false" );
                    break;
                    
                case 0x7c952063: // char
                    sprintf( buffer, "%c", *((char*)obj) );
                    break;
                    
                case 0xf9cba7a0: // unsigned char
                    sprintf( buffer, "%hhu", *((unsigned char*)obj) );
                    break;
                    
                case 0x0b888030: // int
                    sprintf( buffer, "%i", *((int*)obj) );
                    break;
                    
                case 0xb23c93cd: // unsigned int
                    sprintf( buffer, "%u", *((unsigned int*)obj) );
                    break;
                    
                case 0x0f71e19b: // float
                    sprintf( buffer, "%f", *((float*)obj) );
                    break;
                    
                case 0xf93d5b20: // double
                    sprintf( buffer, "%lf", *((double*)obj) );
                    break;
            }
            
            return buffer;
        }
        
        const char* ValueTypeToString( const TypeInfo* type, void* obj, char* buffer, int size )
        {
            // validate params
            
            if( buffer == nullptr || size <= 0 )
            {
                return nullptr;
            }
            
            *buffer = '\0';
            
            if( obj == nullptr || type == nullptr )
            {
                return buffer;
            }
            
            if( type->IsEnum )
            {
                return EnumTypeToString( type, obj, buffer, size );
            }
            else if( type->IsFundamental )
            {
                return FundamentalTypeToString( type, obj, buffer, size );
            }
            
            return buffer;
        }
        
        
        // strings -> value types
        
        static bool EnumTypeFromString( const TypeInfo* type, void* obj, const char* buffer )
        {
            auto pValue = (int*)obj;
            
            if( type->Constants )
            {
                auto c = type->Constants;
                auto i = type->NumMembers;
                
                while( i-- )
                {
                    if( gw_stricmp( buffer, c->Name ) == 0 )
                    {
                        *pValue = c->Value;
                        return true;
                    }
                    
                    ++c;
                }
            }
            
            return sscanf( buffer, "%u", pValue ) == 1;
        }
        
        static bool FundamentalTypeFromString( const TypeInfo* type, void* obj, const char* buffer )
        {
            switch( type->Hash )
            {
                case 0x7c94b391: // bool
                    *((bool*)obj) = gw_stricmp( buffer, "true" ) == 0;
                    return true;
                    
                case 0x7c952063: // char
                    return sscanf( buffer, "%c", (char*)obj ) == 1;
                    
                case 0xf9cba7a0: // unsigned char
                    return sscanf( buffer, "%hhu", (unsigned char*)obj ) == 1;
                    
                case 0x0b888030: // int
                    return sscanf( buffer, "%i", (int*)obj ) == 1;
                    
                case 0xb23c93cd: // unsigned int
                    return sscanf( buffer, "%u", (int*)obj ) == 1;
                    
                case 0x0f71e19b: // float
                    return sscanf( buffer, "%f", (float*)obj ) == 1;
                    
                case 0xf93d5b20: // double
                    return sscanf( buffer, "%lf", (double*)obj ) == 1;
            }
            
            return false;
        }
        
        bool ValueTypeFromString( const TypeInfo* type, void* obj, const char* buffer )
        {
            if( type == nullptr || obj == nullptr || buffer == nullptr )
            {
                return false;
            }
            
            if( type->IsEnum )
            {
                return EnumTypeFromString( type, obj, buffer );
            }
            else if( type->IsFundamental )
            {
                return FundamentalTypeFromString( type, obj, buffer );
            }
            
            return false;
        }
    }
}

