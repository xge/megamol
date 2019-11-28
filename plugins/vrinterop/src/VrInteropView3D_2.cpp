/*
 * VrInteropView3D.cpp
 *
 * Copyright (C) 2008 - 2010, 2018 by VISUS (Universitaet Stuttgart).
 * Alle Rechte vorbehalten.
 */


#include "VrInteropView3D_2.h"

#include "mmcore/view/CallRender3D_2.h"
#include "mmcore/param/BoolParam.h"

#include <iostream>

using namespace megamol::vrinterop;
using namespace megamol::core;

using CameraParamsStore = vislib::graphics::CameraParamsStore;

/*
 * vrinterop::VrInteropView3D::VrInteropView3D
 */
VrInteropView3D_2::VrInteropView3D_2(void) : View3D_2() {
    // init variables
}


/*
 * vrinterop::VrInteropView3D_2::~VrInteropView3D_2
 */
VrInteropView3D_2::~VrInteropView3D_2(void)
{
	this->Release();
}

namespace {
	auto areDimsDifferent = [](auto width1, auto height1, auto width2, auto height2) -> bool
	{
		return
			static_cast<decltype(width2)>(width1) != width2
			|| static_cast<decltype(height2)>(height1) != height2;
	};

	const auto resizeFBO = [&](auto& fbo, const int width, const int height)
	{
		// maybe FBO texture resolution needs to change
		if (areDimsDifferent(width, height, fbo.GetWidth(), fbo.GetHeight()))
		{
			if (fbo.IsValid())
				fbo.Release();
	
			std::cout << "resize FBO to width=" << width << ", height=" << height << std::endl;
			fbo.Create(width, height);
		}
	};
} // namespace

/*
 * vrinterop::VrInteropView3D::Render
 */
void VrInteropView3D_2::Render(const mmcRenderViewContext& context) {

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // to get the Bbox, we issue a Render(). clean it up.

	doBboxDataShare(context);

	bool hasModelPose = m_datasetPoseReceiver.getData<interop::ModelPose>(m_datasetPose);
    hasModelPose = true; // for now, we use relative dataset positioning from unity - no model matrix is sent.
    bool hasCamView = m_stereoViewReceiver.getData<interop::StereoCameraView>(m_stereoCamView);
    static bool hasCamProj = m_camProjectionReceiver.getData<interop::CameraProjection>(m_cameraProjection);

	if (hasCamView && hasCamProj && hasModelPose)
	{
		this->applyCameraConfig(m_stereoLCam, m_stereoCamView.leftEyeView, m_cameraProjection, m_datasetPose);
		this->applyCameraConfig(m_stereoRCam, m_stereoCamView.rightEyeView, m_cameraProjection, m_datasetPose);
    }
	else
		return;

	static int fboVp[4] = { 0, 0, m_cameraProjection.pixelWidth, m_cameraProjection.pixelHeight };
	const auto setFBO = [&](FramebufferObject& fbo)
	{
        Base::overrideViewport = fboVp;
        Base::overrideCall = nullptr; // or else View3D overrides our viewport
        //Base::rendererSlot.CallAs<view::CallRender3D>()->SetCameraParameters(camParams); // does not help setting the viewport we want
        Base::rendererSlot.CallAs<view::CallRender3D>()->SetOutputBuffer(&fbo);
		fbo.Enable();
	};

	if (isNewFbSize(m_cameraProjection.pixelWidth, m_cameraProjection.pixelHeight))
	{
		Base::Resize(m_cameraProjection.pixelWidth, m_cameraProjection.pixelHeight);
        resizeFBO(m_stereoFBO_L, m_cameraProjection.pixelWidth, m_cameraProjection.pixelHeight);
        resizeFBO(m_stereoFBO_R, m_cameraProjection.pixelWidth, m_cameraProjection.pixelHeight);
	}

	setFBO(m_stereoFBO_L);
	renderFromCamera(m_stereoLCam, context);

	setFBO(m_stereoFBO_R);
	renderFromCamera(m_stereoRCam, context);

	broadcastFramebuffer(m_stereoFBO_L, m_stereoImageSender_L);
	broadcastFramebuffer(m_stereoFBO_R, m_stereoImageSender_R);

	//m_stereoFBO_L.DrawColourTexture();
}

void  megamol::vrinterop::VrInteropView3D_2::renderFromCamera(const CameraOpenGL & viewCamera, const mmcRenderViewContext& context)
{
	// we would like to disable the camera updating itself, if that feature wasn't protected...
	// this->cam.markAsUpdated();
	this->cam = viewCamera;
	Base::Render(context);
}

void megamol::vrinterop::VrInteropView3D_2::doBboxDataShare(const mmcRenderViewContext& context)
{
    auto defBbox = m_dataBbox;

	if (!oneTimeDataIsShared)
	{
		// get one time data: bbox
		Base::Render(context); // base render sets bbox of dataset
	}

	//m_dataBbox = this->bboxs.ObjectSpaceBBox();
	const auto bbox = this->bboxs.WorldSpaceBBox();
	// bbox is such a fuckup

    defBbox.min = interop::vec4{
        bbox.GetLeftBottomBack().GetX(),
		bbox.GetLeftBottomBack().GetY(),
		bbox.GetLeftBottomBack().GetZ(),
		0.0f};

	defBbox.max = interop::vec4{
		bbox.GetRightTopFront().GetX(), 
		bbox.GetRightTopFront().GetY(), 
		bbox.GetRightTopFront().GetZ(), 
		0.0f};

	oneTimeDataIsShared = true;
    m_dataBbox = defBbox;

    m_bboxSender.sendData<interop::BoundingBoxCorners>("BoundingBoxCorners", m_dataBbox);
}

void  megamol::vrinterop::VrInteropView3D_2::broadcastFramebuffer(FramebufferObject& fbo, interop::TextureSender& sender)
{
	if (!this->m_spoutSenderActive)
		return;

	sender.sendTexture(fbo.GetColourTextureID(), fbo.GetWidth(), fbo.GetHeight());
}


bool megamol::vrinterop::VrInteropView3D_2::isNewFbSize(unsigned int width, unsigned int height)
{
	const auto newWidth = static_cast<GLint>(width);
	const auto newHeight = static_cast<GLint>(height);

	bool isNew = areDimsDifferent(newWidth, newHeight, m_fbWidth, m_fbHeight);

	m_fbWidth = newWidth;
	m_fbHeight = newHeight;

	if (isNew)
		std::cout << "change FBO size to: w=" << newWidth << ", h=" << newHeight << std::endl;

	return isNew;
}

/*
 * vrinterop::VrInteropView3D_2::create
 */
bool VrInteropView3D_2::create(void) {
	Base::create();

	const std::string baseAdr = {"tcp://127.0.0.1"};
	const std::string recvPort = {":12345"};
	const std::string sendPort = {":12346"};

	// inherit initial camera parameters
	const CameraParamsStore camParams{ *this->cam.Parameters() };
	this->m_stereoLCamParameters = new CameraParamsStore(camParams);
	this->m_stereoRCamParameters = new CameraParamsStore(camParams);
	this->m_stereoLCam = CameraOpenGL(m_stereoLCamParameters);
	this->m_stereoRCam = CameraOpenGL(m_stereoRCamParameters);

	// m_dataBbox - set in render()
    const auto radr = baseAdr + recvPort;
	m_stereoViewReceiver.start(radr, "StereoCameraViewRelative");
	m_camProjectionReceiver.start(radr, "CameraProjection");
	m_datasetPoseReceiver.start(radr, "ModelPose");

	const auto sadr = baseAdr + sendPort;
	m_bboxSender.start(sadr, "BoundingBoxCorners");

	m_stereoFBO_L.Create(1, 1);
	m_stereoFBO_R.Create(1, 1);
    m_stereoImageSender_L.init("/UnityInterop/DefaultNameLeft");
    m_stereoImageSender_R.init("/UnityInterop/DefaultNameRight");

    return true;
}


/*
 * vrinterop::VrInteropView3D_2::release
 */
void VrInteropView3D_2::release(void) {
	Base::release();

	m_stereoFBO_L.Release();
	m_stereoFBO_R.Release();
    m_stereoImageSender_L.destroy();
    m_stereoImageSender_R.destroy();

	m_stereoViewReceiver.stop();
    m_camProjectionReceiver.stop();
    m_datasetPoseReceiver.stop();
	m_bboxSender.stop();
}

void VrInteropView3D_2::applyCameraConfig(CameraOpenGL& cam, const interop::CameraView& view, const interop::CameraProjection proj, interop::ModelPose& pose)
{
	vislib::math::Point<vislib::graphics::SceneSpaceType, 3> position(
		view.eyePos.x,
		view.eyePos.y,
		view.eyePos.z);

	vislib::math::Point<vislib::graphics::SceneSpaceType, 3> lookAt(
		view.lookAtPos.x,
		view.lookAtPos.y,
		view.lookAtPos.z);

	vislib::math::Vector<vislib::graphics::SceneSpaceType, 3> up(
		view.camUpDir.x,
		view.camUpDir.y,
		view.camUpDir.z);

	cam.Parameters()->SetView(position, lookAt, up);

	const float toDegree = 180.f / vislib::math::PI_DOUBLE;
	cam.Parameters()->SetApertureAngle(proj.fieldOfViewY_rad * toDegree);
	cam.Parameters()->SetNearClip(proj.nearClipPlane);
	cam.Parameters()->SetFarClip(proj.farClipPlane);
    cam.Parameters()->SetVirtualViewSize(
		static_cast<vislib::graphics::ImageSpaceType>(proj.pixelWidth),
		static_cast<vislib::graphics::ImageSpaceType>(proj.pixelHeight));
    cam.Parameters()->SetTileRect(vislib::math::Rectangle<float>(0.0f, 0.0f, proj.pixelWidth, proj.pixelHeight));

	// TODO: faking a model matrix is currently broken in MegaMol when rotating the dataset (e.g. in volume raycaster)
	//float scale = pose.scale.x;
    //cam.SetScale(scale);

    //float angle_rad = pose.rotation_axis_angle_rad.w;
    //const auto& ax = pose.rotation_axis_angle_rad;
	//vislib::math::Vector<vislib::graphics::SceneSpaceType, 3> axis(ax.x, ax.y, ax.z);
    //vislib::math::Quaternion<vislib::graphics::SceneSpaceType> rotate(angle_rad, axis);
    //cam.SetRotate(rotate);

	//const auto& pos = pose.translation;
	//vislib::math::Vector<vislib::graphics::SceneSpaceType, 3> translate(pos.x, pos.y, pos.z);
	//cam.SetTranslate(translate);
}

