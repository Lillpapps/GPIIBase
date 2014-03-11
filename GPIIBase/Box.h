#pragma once

class RenderSystem;
class Camera;

class Box
{
public:
	Box();
	~Box();
	bool Intialize(RenderSystem* render_system, Camera* camera);
	void Cleanup();
	void Draw(RenderSystem* render_system);
	int GetVertexBuffer() { return m_vertex_buffer; }
	int GetIndexBuffer() { return m_index_buffer; }
	int GetTexture() { return m_texture; }
	int GetShader() { return m_shader; }
	
private:
	int m_vertex_buffer;
	int m_vertex_format;
	int m_index_buffer;
	int m_texture;
	int m_shader;
	int m_sampler;
};