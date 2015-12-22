////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "json.h"
#include "classes.h"


std::vector< GameObject* > GameObjects;


////////////////////////////////////////////////////////////////////////////////


void Create()
{
    for( int i=0; i < 10; i++ )
    {
        auto go = new GameObject();
        GameObjects.push_back( go->CreateRandom() );
    }
}


////////////////////////////////////////////////////////////////////////////////

void Save( const char* filename )
{
    // create output file
    
    std::cout << "Saving '" << filename << "'" << std::endl;
    
    auto fp = fopen( filename, "wb" );
    
    if( fp == nullptr )
    {
        std::cerr << "Failed to open output file" << std::endl;
        return;
    }
    
    char writeBuffer[ 65536 ];
    
    FileWriteStream os( fp, writeBuffer, sizeof( writeBuffer ) );
    PrettyWriter< FileWriteStream > writer( os );

    
    // save objects via reflection
    
    writer.StartArray();
    
    for( auto o : GameObjects )
    {
        ExampleWrite( writer, o->GetType(), o );
    }
    
    writer.EndArray();
    
    fclose( fp );
}


////////////////////////////////////////////////////////////////////////////////

void Load( const char* filename )
{
    // open file
    
    std::cout << "Loading '" << filename << "'" << std::endl;
    
    auto fp = fopen( filename, "rb" );
    
    if( fp == nullptr )
    {
        std::cerr << "Failed to open input file" << std::endl;
        return;
    }
    
    char readBuffer[ 65536 ];
    FileReadStream is( fp, readBuffer, sizeof( readBuffer ) );
    
    // parse document
    
    Document doc;
    doc.ParseStream( is );
    
    if( doc.IsArray() )
    {
        std::string ObjTag = "GameObject";
        
        for( auto itr = doc.Begin(); itr != doc.End(); ++itr )
        {
            const Document::GenericValue& node = *itr;
            
            if( node.HasMember( "Type" ) && ObjTag == node[ "Type" ].GetString() )
            {
                if( auto obj = ExampleRead( nullptr, *itr ) )
                {
                    auto go = reinterpret_cast<GameObject*>( obj );
                    GameObjects.push_back( go );
                }
            }
        }
    }
    else
    {
        std::cerr << "Was expecting an array" << std::endl;
    }
    
    fclose(fp);    
}


////////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[] )
{
    srand( (unsigned int) time( nullptr ) );
    
    auto filename = "gameobjects.json";
    
    /**
    Create();
    Save( filename );
    /*/
    Load( filename );
    /**/
    
    return 0;
}
