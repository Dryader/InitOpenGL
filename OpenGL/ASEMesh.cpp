#include "ASEMesh.h"

using namespace ASEMeshes;

ASEMesh::ASEMesh(const char* _fileName)
{
    FileName = gcnew String(_fileName);
    Materials = gcnew List<Material^>();
    GeoObjects = gcnew List<GeoObject^>();
}

void ASEMesh::ParseASEFile()
{
    String^ ASEFile = File::ReadAllText(FileName);
    ASEFile = ASEFile->Replace("\r", String::Empty);
    ASEFile = ASEFile->Replace('\t', ' ');

    cli::array<String^>^ splitTerm = gcnew cli::array<String^>(1);
    splitTerm[0] = "*MATERIAL ";
    cli::array<String^>^ MaterialSections = ASEFile->Split(splitTerm, StringSplitOptions::RemoveEmptyEntries);
    ParseMaterials(MaterialSections);

    splitTerm[0] = "*GEOMOBJECT {";
    cli::array<String^>^ GeoObjectsSection = ASEFile->Split(splitTerm, StringSplitOptions::RemoveEmptyEntries);
    ParseGeoObjects(GeoObjectsSection);
}

void ASEMesh::ExtractVectorList(List<Vec3^>^ _list, String^ _tag, cli::array<String^>^ _lines, int& _lineFound)
{
    String^ info;
    while ((info = ExtractValue(_lines, _tag, _lineFound)) != String::Empty)
    {
        info = info->Remove(0, info->IndexOf(' ') + 1);
        _list->Add(ParseStringToVec3(info));
    }
}

Vec3^ ASEMesh::ParseStringToVec3(String^ _vectorS)
{
    cli::array<String^>^ vecS = _vectorS->Split(' ');
    return gcnew Vec3(float::Parse(vecS[0]), float::Parse(vecS[1]), float::Parse(vecS[2]));
}

String^ ASEMesh::ExtractValue(cli::array<String^>^ _lines, String^ _tag, int& _lineFound)
{
    while (_lineFound < _lines->Length - 1)
    {
        _lineFound++;
        if (_lines[_lineFound]->Contains(_tag))
        {
            String^ s = _lines[_lineFound];
            s = s->Replace(_tag, String::Empty);
            s = s->Replace("\"", String::Empty);
            s = s->Trim();
            return s;
        }
    }

    _lineFound = -1;
    return String::Empty;
}

void ASEMesh::ParseMaterials(cli::array<String^>^ _materials)
{
    for (int count = 1; count < _materials->Length; count++)
    {
        int lineFound = -1;
        cli::array<String^>^ lines = _materials[count]->Split('\n');
        Material^ m = gcnew Material();
        m->Index = int::Parse(ExtractValue(lines, " {", lineFound));
        m->Name = ExtractValue(lines, "*MATERIAL_NAME ", lineFound);
        m->Class = ExtractValue(lines, "*MATERIAL_CLASS ", lineFound);
        m->AmbientColor = ParseStringToVec3(ExtractValue(lines, "*MATERIAL_AMBIENT ", lineFound));
        m->DiffuseColor = ParseStringToVec3(ExtractValue(lines, "*MATERIAL_DIFFUSE ", lineFound));
        m->SpecularColor = ParseStringToVec3(ExtractValue(lines, "*MATERIAL_SPECULAR ", lineFound));
        m->Shine = float::Parse(ExtractValue(lines, "*MATERIAL_SHINE ", lineFound));
        m->ShineStrength = float::Parse(ExtractValue(lines, "*MATERIAL_SHINESTRENGTH ", lineFound));
        m->Transparancy = float::Parse(ExtractValue(lines, "*MATERIAL_TRANSPARENCY ", lineFound));
        m->WireSize = float::Parse(ExtractValue(lines, "*MATERIAL_WIRESIZE ", lineFound));
        m->Shading = ExtractValue(lines, "*MATERIAL_SHADING ", lineFound);

        cli::array<String^>^ qx = gcnew cli::array<String^>(1);
        qx[0] = "\n  *MAP_";
        cli::array<String^>^ maps = _materials[count]->Split(qx, StringSplitOptions::RemoveEmptyEntries);
        ParseMaps(maps, m);

        Materials->Add(m);
    }
}

void ASEMesh::ParseMaps(cli::array<String^>^ _maps, Material^ _material)
{
    for (int count = 1; count < _maps->Length; count++)
    {
        int lineFound = -1;
        cli::array<String^>^ lines = _maps[count]->Split('\n');
        Map^ m = gcnew Map();
        m->Name = ExtractValue(lines, " {", lineFound);
        m->TextureFileName = ExtractValue(lines, "*BITMAP ", lineFound);
        m->UVW_U_Offset = float::Parse(ExtractValue(lines, "*UVW_U_OFFSET ", lineFound));
        m->UVW_V_Offset = float::Parse(ExtractValue(lines, "*UVW_V_OFFSET ", lineFound));
        m->UVW_U_Tiling = float::Parse(ExtractValue(lines, "*UVW_U_TILING ", lineFound));
        m->UVW_V_Tiling = float::Parse(ExtractValue(lines, "*UVW_V_TILING ", lineFound));

        _material->Maps->Add(m);
    }
}

void ASEMesh::ParseGeoObjects(cli::array<String^>^ _geoObjects)
{
    for (int count = 1; count < _geoObjects->Length; count++)
    {
        int lineFound = -1;
        cli::array<String^>^ lines = _geoObjects[count]->Split('\n');

        GeoObject^ g = gcnew GeoObject();
        g->Name = ExtractValue(lines, "*NODE_NAME ", lineFound);
        g->MeshI->TimeValue = int::Parse(ExtractValue(lines, "*TIMEVALUE ", lineFound));
        g->MeshI->NumVertex = int::Parse(ExtractValue(lines, "*MESH_NUMVERTEX ", lineFound));
        g->MeshI->NumFaces = int::Parse(ExtractValue(lines, "*MESH_NUMFACES ", lineFound));

        ExtractVectorList(g->MeshI->Vertices, "*MESH_VERTEX ", lines, lineFound);

        String^ info;
        while ((info = ExtractValue(lines, "*MESH_FACE ", lineFound)) != String::Empty)
        {
            cli::array<String^>^ values = info->Split(':');
            cli::array<String^>^ v1 = values[2]->Trim()->Split(' ');
            cli::array<String^>^ v2 = values[3]->Trim()->Split(' ');
            cli::array<String^>^ v3 = values[4]->Trim()->Split(' ');
            g->MeshI->Faces->Add(gcnew Vec3(float::Parse(v1[0]), float::Parse(v2[0]), float::Parse(v3[0])));
        }

        ExtractVectorList(g->MeshI->TexVertices, "*MESH_TVERT ", lines, lineFound);
        ExtractVectorList(g->MeshI->TexFaces, "*MESH_TFACE ", lines, lineFound);
        ExtractVectorList(g->MeshI->VertexNormals, "*MESH_VERTEXNORMAL ", lines, lineFound);
        g->MaterialID = Int32::Parse(ExtractValue(lines, "*MATERIAL_REF", lineFound));

        GeoObjects->Add(g);
    }
}
