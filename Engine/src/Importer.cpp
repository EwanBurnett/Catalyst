#include "../inc/IO/Importer.h"
#pragma warning(disable : 4996) //for mbstowcs

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdlib.h>

constexpr bool FLIP_UV_MAPS = true;

/**
 * \brief Determines whether the file is a serialized asset or not.
 * \param path The path to test
 * \return True if the path is to a serialized asset, false otherwise
 */
bool IsAsset(const std::basic_string<char> path)
{
    
    if(path.ends_with(".Asset"))
    {
        return true;
    }

    return false;
}


template <typename T>
void WriteData(std::fstream& file, uint64_t count, uint64_t byteWidth, T data)
{
    uint64_t dataSize = byteWidth * count;
    T* d = new T;
    *d = data;

    //Write data
    file.write((char*)d, dataSize);
    delete d;
}
template <typename T>
void WriteData(std::fstream& file, uint64_t count, uint64_t byteWidth, T* data)
{
    uint64_t dataSize = byteWidth * count;
  
    //Write data
    file.write((char*)data, dataSize);

}

template <typename T>
T ReadData(std::ifstream& file, uint64_t size, uint64_t& offset)
{
    file.seekg(offset);
    offset += size;

    char* buffer = new char[size];
    
    T out = {};
    file.read(buffer, size);

    memcpy(&out, buffer, size);
    delete[] buffer;
    return out;
}

template <typename T>
void ReadData(std::ifstream& file, uint64_t size, uint64_t& offset, uintptr_t buffer)
{
    file.seekg(offset);
    offset += size;

    file.read((char*)buffer, size);       
}

/**
 * \brief Imports a model from file. (SLOW)
 * \param filePath The path to the Model to import
 * \return The imported model
 */
Engine::Model ImportModel(std::basic_string<char> filePath)
{

    Engine::Model output = {};
    //Initialize an Asset Importer
        Assimp::Importer importer;

        uint32_t flags =
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenSmoothNormals |
            aiProcess_FixInfacingNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_SortByPType |
            aiProcess_FlipWindingOrder |
            aiProcess_MakeLeftHanded ;

        if constexpr (FLIP_UV_MAPS == true) {
            flags |= aiProcess_FlipUVs;
        }

        //Initialize the asset importer scene
        const aiScene* scene = importer.ReadFile(filePath, flags);

        ERR(scene == nullptr, ("File %s was unable to be read.", filePath.c_str()));  

        //For each mesh, create a mesh filter and a mesh renderer.
        if (scene->HasMeshes()) {
            for (uint32_t sceneMesh = 0; sceneMesh < scene->mNumMeshes; sceneMesh++) {

                Engine::MeshFilter m;
                const aiMesh* mesh = scene->mMeshes[sceneMesh];

                m.Name = mesh->mName.C_Str();
                m.MaterialIndex = mesh->mMaterialIndex;

                //Load Mesh Vertices
                m.Vertices.resize(mesh->mNumVertices);
                for (size_t i = 0; i < mesh->mNumVertices; i++) {
                    m.Vertices[i] = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
                }

                //Load Mesh Normals
                if (mesh->HasNormals()) {
                    m.Normals.resize(mesh->mNumVertices);
                    for (size_t i = 0; i < mesh->mNumVertices; i++) {
                        m.Normals[i] = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
                    }
                }

                //Load Mesh Tangents and Binormals
                if (mesh->HasTangentsAndBitangents()) {
                    m.Tangents.resize(mesh->mNumVertices);
                    m.Binormals.resize(mesh->mNumVertices);
                    for (size_t i = 0; i < mesh->mNumVertices; i++) {
                        m.Tangents[i] = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                        m.Binormals[i] = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
                    }
                }

                //Load Mesh Vertex Colours
                for (size_t i = 0; i < mesh->GetNumColorChannels(); i++)
                {
                    m.VertexColours.resize(mesh->mNumVertices);
                    const aiColor4D* meshColours = mesh->mColors[i];

                    for (size_t j = 0; j < mesh->mNumVertices; j++)
                    {
                        m.VertexColours[i] = { meshColours->r, meshColours->g, meshColours->g, meshColours->a };
                    }
                }


                //Load Mesh Texture Coords
                //TODO: Support additional UV Channels + 3D tex coords
                for (size_t i = 0; i < mesh->GetNumUVChannels(); i++)
                {
                    m.TexCoords.resize(mesh->mNumVertices);


                    for (size_t j = 0; j < mesh->mNumVertices; j++)
                    {
                        const aiVector3D* meshCoords = mesh->mTextureCoords[i] + j;
                        m.TexCoords[j] = { meshCoords->x, meshCoords->y };
                    }
                }

                //Load Mesh Faces and Indices
                m.FaceCount = mesh->mNumFaces;
                m.Indices.reserve(m.FaceCount * 3);

                for (size_t i = 0; i < m.FaceCount; i++)
                {
                    aiFace* face = &mesh->mFaces[i];
                    for (size_t j = 0; j < face->mNumIndices; j++)
                    {
                        m.Indices.emplace_back(face->mIndices[j]);
                    }
                }

                //Append the mesh to the output.
                output.meshes.push_back(m);
                output.renderers.push_back({.material = new Engine::Basic});
            }
        }

        //Add the Materials from the scene
        if (scene->HasMaterials()) {
            for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
                aiMaterial* material = scene->mMaterials[i];

                //Determine which shading model to use
                int shaderModeIdx = 0;
                material->Get(AI_MATKEY_SHADING_MODEL,shaderModeIdx);
                aiShadingMode shaderMode = (aiShadingMode)shaderModeIdx;
                Engine::EShaderType shaderType = Engine::EShaderType::Basic;
                std::basic_string tech = "";

                aiColor3D v3;
                float v1;

                aiString texName;

                Engine::MaterialData* mat = {};

                switch (shaderMode)
                {
                case(aiShadingMode_Flat):   //TODO: Write Flat Shader
                    mat = new Engine::Basic;
                    break;
                case(aiShadingMode_Gouraud):   //TODO: Write Gouraud Shader
                    mat = new Engine::Basic;
                    break;
                case(aiShadingMode_Phong):   //TODO: Write Phong Shader
                    mat = new Engine::Basic;
                    break;
                
                case(aiShadingMode_Blinn):
                {
                    mat = new Engine::Blinn;
                    shaderType = Engine::EShaderType::Blinn;
                    //Set each material attribute appropriately
                    //Diffuse Colour
                    material->Get(AI_MATKEY_COLOR_DIFFUSE, v3);
                    material->Get(AI_MATKEY_OPACITY, v1);
                    ((Engine::Blinn*)mat)->Diffuse = { (uint8_t)(v3.r * 255), (uint8_t)(v3.g * 255), (uint8_t)(v3.b * 255), (uint8_t)(v1 * 255) };

                    //Ambient Colour
                    material->Get(AI_MATKEY_COLOR_AMBIENT, v3);
                    ((Engine::Blinn*)mat)->Ambient = { (uint8_t)(v3.r * 255), (uint8_t)(v3.g * 255), (uint8_t)(v3.b * 255), 0xff };

                    //Specular Colour
                    material->Get(AI_MATKEY_COLOR_SPECULAR, v3);
                    material->Get(AI_MATKEY_SHININESS, v1);
                    ((Engine::Blinn*)mat)->Specular = { (uint8_t)(v3.r * 255), (uint8_t)(v3.g * 255), (uint8_t)(v3.b * 255), (uint8_t)(v1 * 255) };

                    //Specular Hilight
                    material->Get(AI_MATKEY_SHININESS_STRENGTH, v1);
                    ((Engine::Blinn*)mat)->SpecularPower = v1 * 255.0f;

                    //Texture References
                    //Diffuse Map
                    if (material->GetTexture(aiTextureType_DIFFUSE, i, &texName) == AI_SUCCESS) {
                        ((Engine::Blinn*)mat)->DiffuseMap = Engine::StringToWString(texName.C_Str()).c_str();
                        tech = "Textured";
                    }

                    //Specular Map
                    if (material->GetTexture(aiTextureType_SPECULAR, i, &texName) == AI_SUCCESS) {
                        ((Engine::Blinn*)mat)->SpecularMap = Engine::StringToWString(texName.C_Str()).c_str();
                        tech = "Textured";
                    }

                    //Normal Map
                    if (material->GetTexture(aiTextureType_NORMALS, i, &texName) == AI_SUCCESS) {
                        ((Engine::Blinn*)mat)->NormalMap = Engine::StringToWString(texName.C_Str()).c_str();
                        tech = "Textured";
                    }
                }
                break;
                case(aiShadingMode_Toon):   //TODO: Write Toon Shader
                    mat = new Engine::Basic;
                    break;
                case(aiShadingMode_OrenNayar):   //TODO: Write OrenNayar Shader
                    mat = new Engine::Basic;
                    break;
                case(aiShadingMode_Minnaert):   //TODO: Write Minnaert Shader
                    mat = new Engine::Basic;
                    break;
                case(aiShadingMode_CookTorrance):   //TODO: Write Cook-Torrance Shader
                    mat = new Engine::Basic;
                    break;
                case(aiShadingMode_NoShading):   //TODO: Write No Shading Shader
                    mat = new Engine::Basic;
                    break;
                case(aiShadingMode_Fresnel):   //TODO: Write Fresnel Shader
                    mat = new Engine::Basic;
                    break;

                default:
                    mat = new Engine::Basic;
                    break;
                }

                output.renderers[i] = {
                    Engine::EPrimitiveTopology::TriangleList,
                    shaderType, 
                    mat,
                    tech,
                };
            }
        }

    return(output);
}

void SerializeMaterialData(std::fstream& outfile, const Engine::MeshRenderer& renderer)
{
    Engine::MaterialData* mat = renderer.material;
    if (renderer.material != nullptr)
    {
        if (renderer.shader == Engine::EShaderType::Basic) 
        {
            //Diffuse (4 bytes)
            WriteData<Engine::Colour>(outfile, 1, sizeof(Engine::Colour), ((Engine::Basic*)mat)->Diffuse);
        }
        else if (renderer.shader == Engine::EShaderType::Blinn) 
        {
            //Ambient (4 bytes)
            WriteData<Engine::Colour>(outfile, 1, sizeof(Engine::Colour), ((Engine::Blinn*)mat)->Ambient);
            //Diffuse (4 bytes)
            WriteData<Engine::Colour>(outfile, 1, sizeof(Engine::Colour), ((Engine::Blinn*)mat)->Diffuse);
            //Specular (4 bytes)
            WriteData<Engine::Colour>(outfile, 1, sizeof(Engine::Colour), ((Engine::Blinn*)mat)->Specular);
            //Specular Power (4 bytes)
            WriteData<float>(outfile, 1, sizeof(float), ((Engine::Blinn*)mat)->SpecularPower);

            //Diffuse Map (8 bytes *)
            WriteData<uint64_t>(outfile, 1, sizeof(uint64_t), ((Engine::Blinn*)mat)->DiffuseMap.length());
            WriteData<const wchar_t>(outfile, ((Engine::Blinn*)mat)->DiffuseMap.length(), sizeof(wchar_t), ((Engine::Blinn*)mat)->DiffuseMap.data());
            //Normal Map (8 bytes *)
            WriteData<uint64_t>(outfile, 1, sizeof(uint64_t), ((Engine::Blinn*)mat)->NormalMap.length());
            WriteData<const wchar_t>(outfile, ((Engine::Blinn*)mat)->NormalMap.length(), sizeof(wchar_t), ((Engine::Blinn*)mat)->NormalMap.data());
            //Specular Map (8 bytes *)
            WriteData<uint64_t>(outfile, 1, sizeof(uint64_t), ((Engine::Blinn*)mat)->SpecularMap.length());
            WriteData<const wchar_t>(outfile, ((Engine::Blinn*)mat)->SpecularMap.length(), sizeof(wchar_t), ((Engine::Blinn*)mat)->SpecularMap.data());
            


        }
    }
}

void SerializeModelData(const Engine::Model& model, const std::basic_string<char> fileName, uint16_t version = ASSET_VERSION)
{
    //Create a file stream
    std::fstream outFile(fileName.c_str(), std::ios::out | std::ios::binary);

    if (outFile.is_open())
    {
        //Write the version of the file (2 bytes)
        WriteData<uint16_t>(outFile, 1, 2, &version);

        //Write the source path (8 bytes *)
        uint64_t nameWidth = model.source.length();
        WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), &nameWidth);
        WriteData<const char>(outFile, model.source.length(), sizeof(char), model.source.c_str());

        //Write the number of meshes (8 bytes)
        uint64_t numMeshes = model.meshes.size();
        WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), &numMeshes);

        //Write the mesh data 
        for (auto mesh : model.meshes)
        {
            //Mesh name (8 bytes *)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), mesh.Name.length());
            WriteData<const char>(outFile, mesh.Name.length(), sizeof(char), mesh.Name.c_str());

            //Vertices (10 bytes *)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), mesh.Vertices.size());
            WriteData<uint16_t>(outFile, 1, sizeof(uint16_t), sizeof(Engine::Vector3f));
            WriteData<Engine::Vector3f>(outFile, mesh.Vertices.size(), sizeof(Engine::Vector3f), mesh.Vertices.data());

            //Indices (10 bytes *)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), mesh.Indices.size());
            WriteData<uint16_t>(outFile, 1, sizeof(uint16_t), sizeof(uint32_t));
            WriteData<uint32_t>(outFile, mesh.Indices.size(), sizeof(uint32_t), mesh.Indices.data());

            //TexCoords (10 bytes *)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), mesh.TexCoords.size());
            WriteData<uint16_t>(outFile, 1, sizeof(uint16_t), sizeof(Engine::Vector2f));
            WriteData<Engine::Vector2f>(outFile, mesh.TexCoords.size(), sizeof(Engine::Vector2f), mesh.TexCoords.data());

            //Normals (10 bytes *)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), mesh.Normals.size());
            WriteData<uint16_t>(outFile, 1, sizeof(uint16_t), sizeof(Engine::Vector3f));
            WriteData<Engine::Vector3f>(outFile, mesh.Normals.size(), sizeof(Engine::Vector3f), mesh.Normals.data());

            //Tangents (10 bytes *)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), mesh.Tangents.size());
            WriteData<uint16_t>(outFile, 1, sizeof(uint16_t), sizeof(Engine::Vector3f));
            WriteData<Engine::Vector3f>(outFile, mesh.Tangents.size(), sizeof(Engine::Vector3f), mesh.Tangents.data());

            //Binormals (10 bytes *)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), mesh.Binormals.size());
            WriteData<uint16_t>(outFile, 1, sizeof(uint16_t), sizeof(Engine::Vector3f));
            WriteData<Engine::Vector3f>(outFile, mesh.Binormals.size(), sizeof(Engine::Vector3f), mesh.Binormals.data());

            //Vertex Colours (10 bytes *)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), mesh.VertexColours.size());
            WriteData<uint16_t>(outFile, 1, sizeof(uint16_t), sizeof(Engine::Vector4f));
            WriteData<Engine::Vector4f>(outFile, mesh.VertexColours.size(), sizeof(Engine::Vector4f), mesh.VertexColours.data());
        }

        //Write the renderer data
        for(auto renderer : model.renderers)
        {
            //Shader Type (8 bytes)
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), (uint64_t)renderer.shader);

            //Shader Technique
            WriteData<uint64_t>(outFile, 1, sizeof(uint64_t), renderer.technique.length());
            WriteData<const char>(outFile, renderer.technique.length(), sizeof(char), renderer.technique.data());

            //Material Data
            SerializeMaterialData(outFile, renderer);
        }

    }
}

bool VersionCheck(const uint16_t version)
{
    if(version < ASSET_VERSION)
    {
        return false;
    }
    return true;
}

Engine::Model LoadModelAsset(std::basic_string<char> filePath)
{
    Engine::Model m = {};
    

    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if (inFile.is_open()) {
        uint64_t offset = 0; //= ReadData<uint64_t>(inFile, sizeof(uint64_t), offset);

        //Perform a Version Check
        uint16_t version = ReadData<uint16_t>(inFile, sizeof(uint16_t), offset);;
        if (VersionCheck(version) == false)
        {
            //Log("Import Failed!\n<%s> Asset Version is invalid or deprecated.
            return m;
        }

        //Load the model source path
        uint64_t sourcePathLen = ReadData<uint64_t>(inFile, sizeof(uint64_t), offset);
        m.source.resize(sourcePathLen);
        ReadData<char>(inFile, sizeof(char) * sourcePathLen, offset, (uintptr_t)m.source.data());

        //Load Mesh Data
        uint64_t numMeshes = ReadData<uint64_t>(inFile, sizeof(uint64_t), offset);
        m.meshes.resize(numMeshes);

        uint16_t byteWidth = 0;

        for (auto mesh = 0; mesh < numMeshes; mesh++)
        {
            //Load Mesh Name
            m.meshes[mesh].Name.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<char>(inFile, m.meshes[mesh].Name.length() * sizeof(char), offset, (uintptr_t)m.meshes[mesh].Name.data());

            //Load Vertices
            m.meshes[mesh].Vertices.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<uint16_t>(inFile, sizeof(uint16_t), offset, (uintptr_t)&byteWidth);
            ReadData<Engine::Vector3f>(inFile,  m.meshes[mesh].Vertices.size() * byteWidth, offset, (uintptr_t)m.meshes[mesh].Vertices.data());            

            //Load Indices
            m.meshes[mesh].Indices.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<uint16_t>(inFile, sizeof(uint16_t), offset, (uintptr_t)&byteWidth);
            ReadData<uint32_t>(inFile, m.meshes[mesh].Indices.size() * byteWidth, offset, (uintptr_t)m.meshes[mesh].Indices.data());

            //Load Texcoords
            m.meshes[mesh].TexCoords.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<uint16_t>(inFile, sizeof(uint16_t), offset, (uintptr_t)&byteWidth);
            ReadData<Engine::Vector2f>(inFile, m.meshes[mesh].TexCoords.size() * byteWidth, offset, (uintptr_t)m.meshes[mesh].TexCoords.data());

            //Load Normals
            m.meshes[mesh].Normals.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<uint16_t>(inFile, sizeof(uint16_t), offset, (uintptr_t)&byteWidth);
            ReadData<Engine::Vector3f>(inFile, m.meshes[mesh].Normals.size() * byteWidth, offset, (uintptr_t)m.meshes[mesh].Normals.data());

            //Load Tangents
            m.meshes[mesh].Tangents.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<uint16_t>(inFile, sizeof(uint16_t), offset, (uintptr_t)&byteWidth);
            ReadData<Engine::Vector3f>(inFile, m.meshes[mesh].Tangents.size() * byteWidth, offset, (uintptr_t)m.meshes[mesh].Tangents.data());

            //Load Binormals
            m.meshes[mesh].Binormals.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<uint16_t>(inFile, sizeof(uint16_t), offset, (uintptr_t)&byteWidth);
            ReadData<Engine::Vector3f>(inFile, m.meshes[mesh].Binormals.size() * byteWidth, offset, (uintptr_t)m.meshes[mesh].Binormals.data());

            //Load VertexColours
            m.meshes[mesh].VertexColours.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<uint16_t>(inFile, sizeof(uint16_t), offset, (uintptr_t)&byteWidth);
            ReadData<Engine::Vector4f>(inFile, m.meshes[mesh].VertexColours.size() * byteWidth, offset, (uintptr_t)m.meshes[mesh].VertexColours.data());
        }

        //Load Material Data
        m.renderers.resize(numMeshes);
        for(auto renderer = 0; renderer < numMeshes; renderer++)
        {
            Engine::EShaderType type = (Engine::EShaderType)ReadData<uint64_t>(inFile, sizeof(uint64_t), offset);
            m.renderers[renderer].shader = type;
            
            std::basic_string<wchar_t> wstrBuf;
            std::basic_string<char> strBuf;

            strBuf.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
            ReadData<char>(inFile, strBuf.size(), offset, (uintptr_t)strBuf.data());
            m.renderers[renderer].technique = strBuf;

            if(type == Engine::EShaderType::Basic)
            {
                auto mat = new Engine::Basic;
                mat->Diffuse = ReadData<Engine::Colour>(inFile, sizeof(Engine::Colour), offset);

                m.renderers[renderer].material = mat;
            }

            else if(type == Engine::EShaderType::Blinn){
                auto mat = new Engine::Blinn;
                mat->Ambient = ReadData<Engine::Colour>(inFile, sizeof(Engine::Colour), offset);
                mat->Diffuse = ReadData<Engine::Colour>(inFile, sizeof(Engine::Colour), offset);
                mat->Specular = ReadData<Engine::Colour>(inFile, sizeof(Engine::Colour), offset);
                mat->SpecularPower = ReadData<float>(inFile, sizeof(float), offset);

                //Texture setup
                

                //Diffuse Map
                wstrBuf.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
                ReadData<wchar_t>(inFile, wstrBuf.size() * 2, offset, (uintptr_t)wstrBuf.data());
                mat->DiffuseMap = wstrBuf;

                //Normal Map
                wstrBuf.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
                ReadData<wchar_t>(inFile, wstrBuf.size() * 2, offset, (uintptr_t)wstrBuf.data());
                mat->NormalMap = wstrBuf;

                //Specular Map
                //texNameLength = ReadData<uint64_t>(inFile, sizeof(uint64_t), offset);
                wstrBuf.resize(ReadData<uint64_t>(inFile, sizeof(uint64_t), offset));
                ReadData<wchar_t>(inFile, wstrBuf.size() * 2, offset, (uintptr_t)wstrBuf.data());
                mat->SpecularMap = wstrBuf;

                m.renderers[renderer].material = mat;
            }

        }
    }

    return m;
}


void Engine::Importer::LoadFromFile(Model& model, const std::basic_string<char>& filePath)
{
    Engine::Log("Loading model %s...\n", filePath.c_str());
    Time timer;
    timer.Reset();
    timer.Tick();

    //If the file is a serialized asset, deserialize it.
    if(IsAsset(filePath))
    {
        model = LoadModelAsset(filePath);
    }
    //Else, import it from its source.
    else
    {
        model = ImportModel(filePath);
    }
    timer.Tick();
    Engine::Log("Finished loading model %s in %fs\n", filePath.c_str(), timer.DeltaTime());
}


Engine::Model Engine::Importer::LoadFromFile(const std::basic_string<char>& filePath)
{
    Model out = {};

    LoadFromFile(out, filePath);

    return out;
}

void Engine::Importer::LoadFromFile(Font& font, const std::basic_string<char> filePath)
{
    //Read the BMFont Binary into a font
    //https://www.angelcode.com/products/bmfont/doc/file_format.html#bin

    Font out = {};

    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if (inFile.is_open()) {
        uint64_t offset = 0;

        //Bytes 1-4 can be skipped
        offset += 4;

        //Byte 5 contains the block identifier
        auto block = ReadData<uint8_t>(inFile, sizeof(uint8_t), offset);

        //The first block contains info about the font, including its name  //TODO: Info Block Data
        offset += ReadData<uint32_t>(inFile, sizeof(uint32_t), offset);

        //The second block contains common information to all characters
        block = ReadData<uint8_t>(inFile, sizeof(uint8_t), offset);

        //Retrieve the width and height of the texture from the common block
        offset += 8;
        auto texWidth = ReadData<uint16_t>(inFile, sizeof(uint16_t), offset);
        auto texHeight = ReadData<uint16_t>(inFile, sizeof(uint16_t), offset);

        out.Size.x = texWidth;
        out.Size.y = texHeight;

        offset += 7;

        //The third block contains texture page paths.
        //TODO: page names
        out.m_Bitmap = L"Resources\\Font_0.png";

        block = ReadData<uint8_t>(inFile, sizeof(uint8_t), offset);
        offset += ReadData<uint32_t>(inFile, sizeof(uint32_t), offset);

        //The fourth block contains character data
        block = ReadData<uint8_t>(inFile, sizeof(uint8_t), offset);
        auto size = ReadData<uint32_t>(inFile, sizeof(uint32_t), offset);

        for(uint32_t i = 0; i < size; i+=20)
        {
            Glyph g = {};

            auto id = ReadData<uint32_t>(inFile, sizeof(uint32_t), offset);
            g.x = ReadData<uint16_t>(inFile, sizeof(uint16_t), offset) / (float)texWidth;
            g.y = ReadData<uint16_t>(inFile, sizeof(uint16_t), offset) / (float)texHeight;
            g.width = (float)ReadData<uint16_t>(inFile, sizeof(uint16_t), offset) ;
            g.height = (float)ReadData<uint16_t>(inFile, sizeof(uint16_t), offset) ;
            g.offsetX = ReadData<uint16_t>(inFile, sizeof(uint16_t), offset);
            g.offsetY = ReadData<uint16_t>(inFile, sizeof(uint16_t), offset);
            g.advanceX = ReadData<uint16_t>(inFile, sizeof(uint16_t), offset);

            offset += 2;

            out.m_Glyphs.emplace(id, g);
        }

        //The fifth block contains kerning pairs
        //TODO: kerning pairs

    }

    font = out;
}

void Engine::Importer::ImportModelFromMemory(Model& model, std::basic_string<char> destPath)
{
    if (!destPath.ends_with(".Asset")) {
        destPath.append(".Asset");
    }
    SerializeModelData(model, destPath);
}

Engine::Model Engine::Importer::ImportModelFromFile(const std::basic_string<char>& filePath, std::basic_string<char> destPath)
{
    Engine::Model m = {};

    m.source = filePath;

    //Correct the output file name
    if(destPath.empty())
    {
        destPath = filePath;
    }
    std::basic_string<char> fileName = destPath;
    if(!fileName.ends_with(".Asset"))
    {
        fileName.append(".Asset");
    }
    //TODO: If the asset already exists, generate a .old file.
    std::ifstream file(fileName);
    if (file.good())
    {
        auto newName = fileName;
        newName.append(".Old");
        std::ifstream oldFile(newName);
        if (oldFile.good()) {
            std::remove(newName.c_str());
        }
        auto i = std::rename(fileName.c_str(), newName.c_str());// != 0, ("File <%s> Renaming Failed", newName.c_str());
    }
    Log("Importing <%s> -> <%s>\n", filePath.c_str(), fileName.c_str());


    //Load the model
    LoadFromFile(m, filePath);

    Time timer;
    timer.Reset();

    Engine::Log("Serializing Asset %s...\n", filePath.c_str());

    SerializeModelData(m, fileName);

    timer.Tick();
    Engine::Log("Asset Serialization <%s> finished in %fs\n", fileName.c_str(), timer.DeltaTime());
    
    return m;
}


