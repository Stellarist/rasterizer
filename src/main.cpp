#include <chrono>

#include "Window.hpp"
#include "Pipeline.hpp"

void init();
void tick();

int main(int argc, const char* argv[])
{
	Pipeline::reg(CallbackType::INIT, init);
	Pipeline::reg(CallbackType::TICK, tick);

	Window::instance().run();

	return 0;
}

void init()
{
	auto& pipeline = Pipeline::instance;
	auto  mat = pipeline.shader->getModelMat();
	auto  center = pipeline.model->getBoundingBoxCenter();
	auto  size = pipeline.model->getBoundingBoxSize();
	auto  scale = 0.75f * std::min(pipeline.getWidth() / size.x(), pipeline.getHeight() / size.y());
	mat = Geometry::translate(mat, direct_t(pipeline.getWidth() / 2.f - center.x(), pipeline.getHeight() / 2.f - center.y(), 0.f));
	mat = Geometry::scale(mat, direct_t(scale, scale, scale));
	pipeline.shader->setModelMat(mat);
	pipeline.shader->getOriginModel() = *pipeline.model;
}

void tick()
{
	Pipeline::clear();
	Pipeline::render();

	auto mat = Pipeline::instance.shader->getModelMat();
	auto duration = std::chrono::steady_clock::now().time_since_epoch();
	mat = Geometry::rotate(mat, duration.count(), direct_t(0.f, 1.f, 0.f));
	Pipeline::instance.shader->setModelMat(mat);
}
