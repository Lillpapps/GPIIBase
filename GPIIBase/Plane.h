#pragma once

class RenderSystem;
class Camera;

class Plane
{
public:
	Plane();
	~Plane();
	bool Intialize(RenderSystem* render_system, Camera* camera);
	void Draw(RenderSystem* render_system);
	void Cleanup();
	int GetVertexBuffer() { return m_vertex_buffer; }
	int GetIndexBuffer() { return m_index_buffer; }
	int GetShader() { return m_shader; }

private:
	int m_vertex_buffer;
	int m_vertex_format;
	int m_index_buffer;
	int m_shader;
};

