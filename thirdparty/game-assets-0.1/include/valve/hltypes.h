#ifndef _HLTYPES_H_
#define	_HLTYPES_H_

#include <string>
#include <map>
#include <vector>
#include <set>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "hltexture.h"

typedef std::map<std::string, std::string> KeyValueList;

typedef unsigned char byte;

namespace valve
{

template<typename T> class Array
{
    bool _deleteOnDestruct;
public:
    Array() : count(0), data(nullptr), _deleteOnDestruct(false) { }
    Array(int count) : _deleteOnDestruct(true) { this->Allocate(count); }
    Array(int count, T* data) : count(count), data(data), _deleteOnDestruct(false) { }
    virtual ~Array() { if (this->_deleteOnDestruct) this->Delete(); }

    int count;
    T* data;

    operator T*(void) const { return data; }
    const T& operator[] (int index) const { return this->data[index]; }
    T& operator[] (int index) { return this->data[index]; }

    virtual void Allocate(int count)
    {
        this->count = count;
        this->data = this->count > 0 ? new T[this->count] : nullptr;
    }

    void Map(int count, T* data)
    {
        this->count = count;
        this->data = data;
    }

    virtual void Delete()
    {
        if (this->data != nullptr) delete []this->data;
        this->data = nullptr;
        this->count = 0;
    }
};

#define CHUNK   (4096)

template<class T> class List
{
    int size;
    int count;
    T* data;

public:
    List() : size(CHUNK), data(new T[CHUNK]), count(0) { }
    virtual ~List() { this->Clear(); }

    operator T*(void) const { return data; }
    const T& operator[](int i) const { return data[i]; }
    T& operator[](int i) { return data[i]; }

    int Count() const { return count; }

    void Add(T& src)
    {
        if(count >= size)
        {
            //resize
            T* n = new T[size + CHUNK];
            for(int i = 0; i < size; i++)
                n[i] = data[i];
            delete []data;
            data = n;
            size += CHUNK;
        }

        data[count] = src;
        count++;
    }

    void Clear()
    {
        if (this->data != nullptr)
            delete this->data;
        this->data = nullptr;
        this->size = this->count = 0;
    }
};

typedef struct sVertex
{
    glm::vec3 position;
    glm::vec2 texcoords[2];
    glm::vec3 normal;
    int bone;

} tVertex;

typedef struct sFace
{
    int firstVertex;
    int vertexCount;
    unsigned int lightmap;
    unsigned int texture;

    int flags;
    glm::vec4 plane;

} tFace;

class VertexBuffer
{
private:
    GLuint _vbo;
    GLuint _vao;
    std::vector<tFace> _faces;
    std::vector<Texture*> _textures;
    std::vector<Texture*> _lightmaps;

public:
    VertexBuffer();
    virtual ~VertexBuffer();

    void LoadVertices(const std::vector<tVertex>& vertices);
    void RenderFaces(const std::set<unsigned short>& visibleFaces, GLenum mode = GL_TRIANGLE_FAN);

    std::vector<tFace>& Faces() { return this->_faces; }
    std::vector<Texture*>& Textures() { return this->_textures; }
    std::vector<Texture*>& Lightmaps() { return this->_lightmaps; }

    void Bind();
    void Unbind();
};

enum class ShaderAttributeLocations
{
    Vertex = 0,
    TexCoord,
    LightCoord,
    Normal,
    Bone
};

class Shader
{
protected:
    GLuint _u_projection;
    GLuint _u_view;
    GLuint _u_tex;
    GLuint _u_light;
    GLuint _u_bones;

    // The bones are different for each instance of mdl so we need to manage
    // the data to the uniformbuffer in the instance, not the asset
    unsigned int _bonesBuffer;

public:
    Shader();
    virtual ~Shader();

    void BuildProgram();
    void UseProgram();

    void SetProjectionMatrix(const glm::mat4& m);
    void SetViewMatrix(const glm::mat4& m);
    void BindBones(const glm::mat4 m[], int count);
    void UnbindBones();
protected:
    GLuint _program;

protected:
    virtual const std::map<ShaderAttributeLocations, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program) { }

};

typedef std::string (DataFileLocator)(const std::string& relativeFilename);
typedef Array<byte>& (DataFileLoader)(const std::string& filename);

class Asset
{
protected:
    DataFileLocator& _locator;
    DataFileLoader& _loader;
    VertexBuffer _va;

public:
    Asset(DataFileLocator& locator, DataFileLoader& loader) : _locator(locator), _loader(loader) { }
    virtual ~Asset() { }

    virtual bool Load(const std::string& filename) = 0;
};

class AssetInstance
{
public:
    virtual ~AssetInstance() { }

    virtual void Update(float dt) = 0;
    virtual void Render(const glm::mat4& proj, const glm::mat4& view) = 0;

};

}

#endif	// _HLTYPES_H_

