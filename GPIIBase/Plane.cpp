#include "stdafx.h"
#include "Plane.h"
#include "RenderSystem.h"
#include "Camera.h"


Plane::Plane()
{
	m_vertex_buffer = 0;
	m_index_buffer = 0;
	m_shader = 0;
}


Plane::~Plane()
{
}

bool Plane::Intialize(RenderSystem* render_system, Camera* camera)
{
	// shader
	char vs[1024], ps[1024];
	unsigned int vs_size = File::Size("../data/shader/dx/first_vs.txt");
	unsigned int ps_size = File::Size("../data/shader/dx/first_ps.txt");
	File::Read("../data/shader/dx/first_vs.txt", vs, vs_size);
	File::Read("../data/shader/dx/first_ps.txt", ps, ps_size);
	m_shader = render_system->CreateShader(vs, ps);
	render_system->SelectShader(m_shader);

	// vertex buffer
	struct Vec2 { float x, y; };
	struct Vertex { Vec3 p; Vec2 t; };
	float v = 10.0f;
	float s[5] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
	float t[3] = { 0.0f, 0.5f, 1.0f };
	Vertex vertices[] = {
		// bottom
		{ Vec3(-v, -v, -v), { 0.0f, 1.0f } },
		{ Vec3(v, -v, -v), { 0.0f, 1.0f } },
		{ Vec3(v, -v, v), { 0.0f, 1.0f } },
		{ Vec3(-v, -v, v), { 0.0f, 1.0f } },
	};
	m_vertex_buffer = render_system->CreateVertexBuffer(ACCESS_NONE, vertices, sizeof(vertices));
	render_system->SelectVertexBuffer(m_vertex_buffer);

	short* indices = new short[6];
	indices[0] = 0;
	indices[1] = 3;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 0;

	m_index_buffer = render_system->CreateIndexBuffer(indices, sizeof(short), 6);
	delete[] indices;

	render_system->SelectIndexBuffer(m_index_buffer);

	// vertex format i.e input layout
	FormatDesc desc[] = {
		{ 0, ATTRIB_POSITION, FORMAT_FLOAT, 3 },
		{ 0, ATTRIB_TEXCOORD, FORMAT_FLOAT, 2 },
	};
	m_vertex_format = render_system->CreateVertexFormat(desc, 2, m_shader);
	render_system->SelectVertexFormat(m_vertex_format);

	// we need to apply all changes (settings)
	render_system->Apply();

	return true;
}

void Plane::Draw(RenderSystem* render_system)
{
	render_system->SelectVertexBuffer(m_vertex_buffer);
	render_system->SelectIndexBuffer(m_index_buffer);
	render_system->Apply();
}

void Plane::Cleanup()
{
	
}