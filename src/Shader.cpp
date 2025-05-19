#include "Shader.hpp"
#include "Model.hpp"
#include "Pipeline.hpp"

Shader::Shader() :
    view_pos(direct_t::Zero()),
    model_mat(matrix_t::Identity()),
    view_mat(matrix_t::Identity()),
    projection_mat(matrix_t::Identity())
{
}

direct_t Shader::getViewPos()
{
	return view_pos;
}

matrix_t Shader::getModelMat()
{
	return model_mat;
}

matrix_t Shader::getViewMat()
{
	return view_mat;
}

matrix_t Shader::getProjectionMat()
{
	return projection_mat;
}

Model& Shader::getOriginModel()
{
	return origin_model;
}

Model& Shader::getCurrentModel()
{
	return current_model;
}

void Shader::setViewPos(const direct_t& view_pos)
{
	this->view_pos = view_pos;
}

void Shader::setModelMat(const matrix_t& model_mat)
{
	this->model_mat = model_mat;
}

void Shader::setViewMat(const matrix_t& view_mat)
{
	this->view_mat = view_mat;
}

void Shader::setProjectionMat(const matrix_t& projection_mat)
{
	this->projection_mat = projection_mat;
}

void Shader::use()
{
	Pipeline::instance.shader = this;
}

void Shader::flush()
{
	current_model = origin_model;
}

void Shader::transform()
{
	auto&& mvp_mat = projection_mat * view_mat * model_mat;
	if (mvp_mat == matrix_t::Identity())
		return;

	auto& vertices = current_model.attrib.vertices;
	auto& normals = current_model.attrib.normals;

	for (int i = 0; i < vertices.size(); i += 3) {
		// transform vertices
		auto vec_v = vec4f_t(vertices[i], vertices[i + 1], vertices[i + 2], 1.f);
		vec_v = mvp_mat * vec_v;
		vec_v /= vec_v[3];
		vertices[i] = vec_v[0];
		vertices[i + 1] = vec_v[1];
		vertices[i + 2] = vec_v[2];
	}

	for (int i = 0; i < normals.size(); i += 3) {
		// transform normals
		auto vec_n = vec4f_t(normals[i], normals[i + 1], normals[i + 2], 1.f);
		vec_n = model_mat.inverse().transpose() * vec_n;
		vec_n /= vec_n[3];
		normals[i] = vec_n[0];
		normals[i + 1] = vec_n[1];
		normals[i + 2] = vec_n[2];
	}
}

void Shader::render()
{
	const auto& attrib = current_model.attrib;
	const auto& shapes = current_model.shapes;
	const auto& materials = current_model.materials;
	const auto& textures = current_model.textures;

	// loop over shapes
	for (int s = 0; s < shapes.size(); s++) {
		size_t index_offset = 0;

		// loop over faces
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t     fv = static_cast<size_t>(shapes[s].mesh.num_face_vertices[f]);
			triangle_t triangle;

			// read materials7
			ShaderInfo shader_info;
			int        id = shapes[s].mesh.material_ids[f];
			if (id >= 0) {
				shader_info.ambient = vec3f_t(materials[id].ambient[0], materials[id].ambient[1], materials[id].ambient[2]);
				shader_info.diffuse = vec3f_t(materials[id].diffuse[0], materials[id].diffuse[1], materials[id].diffuse[2]);
				shader_info.specular = vec3f_t(materials[id].specular[0], materials[id].specular[1], materials[id].specular[2]);

				if (!materials[id].diffuse_texname.empty())
					shader_info.textures.push_back(textures.at(materials[id].diffuse_texname));
				if (!materials[id].specular_texname.empty())
					shader_info.textures.push_back(textures.at(materials[id].specular_texname));
				if (!materials[id].bump_texname.empty())
					shader_info.textures.push_back(textures.at(materials[id].bump_texname));

			} else if (!current_model.textures.empty()) {
				for (auto& texture : current_model.textures)
					shader_info.textures.push_back(texture.second);
			}

			// loop over vertices
			for (size_t v = 0; v < fv; v++) {
				auto idx = shapes[s].mesh.indices[index_offset + v];

				// record vertices
				triangle.vertices[v] = vertex_t{
				    attrib.vertices[3 * size_t(idx.vertex_index) + 0],
				    attrib.vertices[3 * size_t(idx.vertex_index) + 1],
				    attrib.vertices[3 * size_t(idx.vertex_index) + 2]};

				// record normals
				if (idx.normal_index >= 0) {
					triangle.normals[v] = normal_t(
					                          attrib.normals[3 * size_t(idx.normal_index) + 0],
					                          attrib.normals[3 * size_t(idx.normal_index) + 1],
					                          attrib.normals[3 * size_t(idx.normal_index) + 2])
					                          .normalized();
				}

				// record textures
				if (idx.texcoord_index >= 0) {
					triangle.texcoords[v] = texcoord_t(
					    attrib.texcoords[2 * size_t(idx.texcoord_index) + 0],
					    attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]);
				}

				// record colors
				triangle.colors[v] = color_t(
				    attrib.colors[3 * size_t(idx.vertex_index) + 0],
				    attrib.colors[3 * size_t(idx.vertex_index) + 1],
				    attrib.colors[3 * size_t(idx.vertex_index) + 2]);
			}
			index_offset += fv;

			shader_info.view_pos = view_pos;
			Pipeline::instance.rasterizer->drawTriangle(triangle, shader_info);
		}
	}
}

color_t Shader::phongShader(const ShaderInfo& shader)
{
	light_t light({20, 20, 20}, {500, 500, 500});

	vec3f_t amb_light_intensity{10, 10, 10};
	vec3f_t eye_pos{0, 0, 10};

	const vec3f_t& ka = shader.ambient;
	const vec3f_t& kd = shader.color;
	const vec3f_t& ks = shader.specular;
	const vec3f_t& point = shader.view_pos;
	const vec3f_t& normal = shader.normal;

	vec3f_t l = (light.position - point).normalized();
	vec3f_t v = (eye_pos - point).normalized();
	vec3f_t h = (l + v).normalized();
	vec3f_t i = light.intensity / (light.position - point).dot(light.position - point);

	vec3f_t ambient = ka.cwiseProduct(amb_light_intensity);
	vec3f_t diffuse = kd.cwiseProduct(i) * std::max(0.0f, normal.dot(l));
	vec3f_t specular = ks.cwiseProduct(i) * pow(std::max(0.0f, normal.dot(h)), 128);
	vec3f_t result_color = ambient + diffuse + specular;

	return result_color;
}

color_t Shader::textureShader(const ShaderInfo& shader)
{
	vec3f_t texture_color = vec3f_t::Identity();
	if (!shader.textures.empty()) {
		texture_color = shader.textures[0]->sample(shader.texcoord.x(), shader.texcoord.y());
		for (auto& texture : shader.textures)
			if (texture->type == TextureType::DIFFUSE)
				texture_color = texture->sample(shader.texcoord.x(), shader.texcoord.y());
		return texture_color;
	}

	light_t light({960, 540, 20}, {500, 500, 500});
	vec3f_t amb_light_intensity{10, 10, 10};
	vec3f_t eye_pos{900, 540, 10};

	const vec3f_t& ka = shader.ambient;
	const vec3f_t& kd = texture_color;
	const vec3f_t& ks = shader.specular;
	const vec3f_t& point = shader.view_pos;
	const vec3f_t& normal = shader.normal;

	vec3f_t l = (light.position - point).normalized();
	vec3f_t v = (eye_pos - point).normalized();
	vec3f_t h = (l + v).normalized();
	vec3f_t i = light.intensity / (light.position - point).dot(light.position - point);
	vec3f_t ambient = ka.cwiseProduct(amb_light_intensity);
	vec3f_t diffuse = kd.cwiseProduct(i) * std::max(0.0f, normal.dot(l));
	vec3f_t specular = ks.cwiseProduct(i) * pow(std::max(0.0f, normal.dot(h)), 128);
	vec3f_t result_color = ambient + diffuse + specular;

	return result_color;
}
