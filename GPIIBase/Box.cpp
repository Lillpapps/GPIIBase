#include "stdafx.h"
#include "Box.h"
#include "Camera.h"
#include "RenderSystem.h"


Box::Box()
{
}


Box::~Box()
{
}

bool Box::Intialize(RenderSystem* render_system, Camera* camera)
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
	float v = 1.0f;
	float s[5] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
	float t[3] = { 0.0f, 0.5f, 1.0f };
	Vertex vertices[] = {
		// front
		{ Vec3(-v, v,-v), { 0.0f, 0.0f } }, // 0, 1, 2, 2, 3, 0,
		{ Vec3( v, v,-v), { 1.0f, 0.0f } },
		{ Vec3( v,-v,-v), { 1.0f, 1.0f } },
		{ Vec3(-v,-v,-v), { 0.0f, 1.0f } },

		// right
		{ Vec3( v, v,-v), { s[1], t[1] } }, // 4, 5, 6, 6, 7, 4
		{ Vec3( v, v, v), { s[2], t[1] }},
		{ Vec3( v,-v, v), { s[2], t[2] }},
		{ Vec3( v,-v,-v), { s[1], t[2] }},

		// back
		{ Vec3( v, v, v), { s[2], t[1] }}, // 8, 9, 10, 10, 11, 8
		{ Vec3(-v, v, v), { s[3], t[1] }},
		{ Vec3(-v,-v, v), { s[3], t[2] }},
		{ Vec3( v,-v, v), { s[2], t[2] }},

		// left
		{ Vec3(-v, v, v), { s[3], t[1] }}, // 12, 13, 15, 15, 16, 12
		{ Vec3(-v, v,-v), { s[4], t[1] }},
		{ Vec3(-v,-v,-v), { s[4], t[2] }},
		{ Vec3(-v,-v, v), { s[3], t[2] }},

		// top
		{ Vec3(-v, v, v), { s[2], t[0] }}, // 16, 17, 18, 18, 19, 16
		{ Vec3( v, v, v), { s[2], t[1] }},
		{ Vec3( v, v,-v), { s[1], t[1] }},
		{ Vec3(-v, v,-v), { s[1], t[0] }},

		// bottom
		{ Vec3(-v, -v, -v), { 0.0f, 1.0f } }, // 20, 21, 22, 22, 23, 20
		{ Vec3(v, -v, -v), { 0.0f, 1.0f } },
		{ Vec3(v, -v, v), { 0.0f, 1.0f } },
		{ Vec3(-v, -v, v), { 0.0f, 1.0f } },
	};
	m_vertex_buffer = render_system->CreateVertexBuffer(ACCESS_NONE, vertices, sizeof(vertices));
	render_system->SelectVertexBuffer(m_vertex_buffer);

	//index buffer
	short* indices = new short[36];
	for(int i = 0, index = 0; i < 6; i++) {
	indices[index++] = i * 4 + 0;
	indices[index++] = i * 4 + 1;
	indices[index++] = i * 4 + 2;
	indices[index++] = i * 4 + 2;
	indices[index++] = i * 4 + 3;
	indices[index++] = i * 4 + 0;
	};
	m_index_buffer = render_system->CreateIndexBuffer(indices, sizeof(short), 36);
	delete[] indices;
	render_system->SelectIndexBuffer(m_index_buffer);

	// vertex format i.e input layout
	FormatDesc desc[] = {
		{ 0, ATTRIB_POSITION, FORMAT_FLOAT, 3 },
		{ 0, ATTRIB_TEXCOORD, FORMAT_FLOAT, 2 },
	};
	m_vertex_format = render_system->CreateVertexFormat(desc, 2, m_shader);
	render_system->SelectVertexFormat(m_vertex_format);

	// fill shaders constantbuffer 
	// note: shader has to be selected before setting shader constants
	struct {
		Mat4 proj;
		Mat4 view;
	} frame;
	Mat4::Translation(frame.view, Vec3(0.0f, 0.0f, 10.0f));
	Mat4::Perspective(frame.proj, 45.0f * 3.141592f / 180.0f, (float)render_system->GetWidth() / (float)render_system->GetHeight(), 0.5f, 1000.0f);
	render_system->SetShaderConstantMatrix("projection", frame.proj);
	render_system->SetShaderConstantMatrix("view", camera->GetView());

	int width = 1024;
	int height = 600;

	// texture
	FILE* file = fopen("../data/texture/crate.png", "rb");
	int components = 0;
	unsigned char* buffer = stbi_load_from_file(file, &width, &height, &components, 4);
	fclose(file);
	m_texture = render_system->CreateTexture(buffer, width, height, components - 1);
	stbi_image_free(buffer);
	render_system->SelectTexture(m_texture);

	// sampler state
	m_sampler = render_system->CreateSamplerState(ADDRESS_CLAMP, ADDRESS_CLAMP, FILTER_LINEAR);
	render_system->SelectSamplerState(m_sampler);

	// we need to apply all changes (settings)
	render_system->Apply();

	return true;
}

void Box::Draw(RenderSystem* render_system)
{
	render_system->SelectShader(m_shader);
	render_system->SelectVertexBuffer(m_vertex_buffer);
	render_system->SelectIndexBuffer(m_index_buffer);
	render_system->SelectTexture(m_texture);
	render_system->Apply();
}

void Box::Cleanup()
{

}