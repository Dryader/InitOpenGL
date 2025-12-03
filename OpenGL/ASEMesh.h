#pragma once
namespace ASEMeshes
{
    using namespace System;
    using namespace System::Collections::Generic;
    using namespace System::IO;

    ref class Vec3
    {
    public:
        Vec3(float _x, float _y, float _z)
        {
            X = _x;
            Y = _y;
            Z = _z;
        };

        property float X;
        property float Y;
        property float Z;
    };

    ref class Map
    {
    public:
        property String^ Name;
        property String^ TextureFileName;
        property float UVW_U_Offset;
        property float UVW_V_Offset;
        property float UVW_U_Tiling;
        property float UVW_V_Tiling;
    };

    ref class Material
    {
    public:
        Material() { Maps = gcnew List<Map^>(); }

        property int Index;
        property String^ Name;
        property String^ Class;
        property Vec3^ AmbientColor;
        property Vec3^ DiffuseColor;
        property Vec3^ SpecularColor;
        property float Shine;
        property float ShineStrength;
        property float Transparancy;
        property float WireSize;
        property String^ Shading;
        property float XP_Falloff;
        property float SelfIllum;
        property String^ Falloff;
        property String^ XP_Type;
        property List<ASEMeshes::Map^>^ Maps;
    };

    ref class MeshInfo
    {
    public:
        MeshInfo()
        {
            Vertices = gcnew List<Vec3^>();
            Faces = gcnew List<Vec3^>();
            TexVertices = gcnew List<Vec3^>();
            TexFaces = gcnew List<Vec3^>();
            VertexNormals = gcnew List<Vec3^>();
        }

        property int TimeValue;

        property int NumVertex;
        property List<Vec3^>^ Vertices;
        property int NumFaces;
        property List<Vec3^>^ Faces;

        property int NumTexVertex;
        property List<Vec3^>^ TexVertices;
        property int NumTVFaces;
        property List<Vec3^>^ TexFaces;

        property List<Vec3^>^ VertexNormals;
    };

    ref class GeoObject
    {
    public:
        GeoObject() { MeshI = gcnew MeshInfo(); }

        property String^ Name;
        property MeshInfo^ MeshI;
        property int MaterialID;
    };

    ref class ASEMesh
    {
    public:
        // Constructors / Destructors
        ASEMesh(const char* _fileName);

        // Methods
        void ParseASEFile();

        // Members
        property String^ FileName;
        property List<Material^>^ Materials;
        property List<GeoObject^>^ GeoObjects;

    private:
        // Methods
        String^ ExtractValue(cli::array<String^>^ _lines, String^ _tag, int& _lineFound);
        void ParseMaps(cli::array<String^>^ _maps, Material^ _material);
        void ExtractVectorList(List<Vec3^>^ _list, String^ _tag, cli::array<String^>^ _lines, int& _lineFound);
        void ParseGeoObjects(cli::array<String^>^ _geoObjects);
        void ParseMaterials(cli::array<String^>^ _materials);
        Vec3^ ParseStringToVec3(String^ _vectorS);
    };
}
