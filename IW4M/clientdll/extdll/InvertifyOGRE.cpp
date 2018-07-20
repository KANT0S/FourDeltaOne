// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Project 'Inverse' - Invertify (merge code)
//
// Initial author: NTAuthority
// Started: 2013-05-14
// ==========================================================

#include "StdInc.h"

#if defined(ENABLE_INVERSE) && defined(INVERSE_OGRE)
#include "InvOgre.h"

#include <d3d9.h>
#include <Ogre.h>
#include <OgreD3D9MultiRenderTarget.h>
#include <OgreD3D9RenderSystem.h>
#include <OgreD3D9Device.h>
#include <OgreD3D9DeviceManager.h>
#include <OgreD3D9RenderWindow.h>
#include <OgreOctreePlugin.h>
#include <OgreFileSystem.h>
#include <OgreDepthBuffer.h>

struct LogListener : Ogre::LogListener {
	virtual void messageLogged (const std::string &message,
		Ogre::LogMessageLevel lml,
		bool maskDebug,
		const std::string &logName,
		bool& skipThisMessage )

	{
		(void)lml;
		(void)logName;
		(void)skipThisMessage;
		if (!maskDebug) Com_Printf(0, "%s\n", message.c_str());
	}
} log_listener;

static Ogre::OctreePlugin* octree;
Ogre::Root* ogre_root;
Ogre::SceneManager* ogre_sm;
Ogre::Camera* ogre_camera;

void Inverse_LoadBSP(const char* filename);

void Inverse_Init()
{
	Ogre::LogManager *lmgr = OGRE_NEW Ogre::LogManager();
	Ogre::Log *ogre_log = OGRE_NEW Ogre::Log("",false,true);
	ogre_log->addListener(&log_listener);

	lmgr->setDefaultLog(ogre_log);
	lmgr->setLogDetail(Ogre::LL_NORMAL);

	ogre_root = OGRE_NEW Ogre::Root("","","");

	octree = OGRE_NEW Ogre::OctreePlugin();
	ogre_root->installPlugin(octree);

	Ogre::D3D9RenderSystem* d3d9_rs = OGRE_NEW Ogre::D3D9RenderSystem(GetModuleHandle(NULL));

	ogre_root->setRenderSystem(d3d9_rs);
	ogre_root->initialise(false);

	IDirect3DDevice9* d3dDevice = *(IDirect3DDevice9**)0x66DEF88;

	Ogre::NameValuePairList params;
	params["externalWindowHandle"] = Ogre::StringConverter::toString((int)FindWindow("IW4", NULL)); // fixme: badbad
	params["externalDevice"] = Ogre::StringConverter::toString((size_t)d3dDevice);

	Ogre::D3D9RenderWindow* ogreWindow = (Ogre::D3D9RenderWindow*)ogre_root->createRenderWindow("invertify", 2560, 1440, false, &params);

	/*Ogre::D3D9DeviceManager* manager = OGRE_NEW Ogre::D3D9DeviceManager();
	Ogre::D3D9Device* device = manager->getDeviceFromD3D9Device(d3dDevice);

	ogreWindow->setDevice(device);
	device->attachRenderWindow(ogreWindow);*/

	ogre_sm = ogre_root->createSceneManager("OctreeSceneManager");
	ogre_camera = ogre_sm->createCamera("camera");

	ogre_sm->setAmbientLight(Ogre::ColourValue());

	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Inverse");

	Inverse_LoadBSP("mp_inv_dev.bsp");
}

class RefVP : public Ogre::RenderTarget
{
private:
	IDirect3DSurface9* buffer;
public:
	RefVP(IDirect3DSurface9* bb)
		: Ogre::RenderTarget()
	{
		buffer = bb;

		setDepthBufferPool(Ogre::DepthBuffer::POOL_NO_DEPTH);
	}

	void getCustomAttribute(const Ogre::String& name, void* pData)
	{
		if(name == "DDBACKBUFFER")
		{
			IDirect3DSurface9 ** pSurf = (IDirect3DSurface9 **)pData;
			//pSurf[0] = buffer;


			return;
		}
	}

	virtual void copyContentsToMemory(const Ogre::PixelBox &dst, FrameBuffer buffer)
	{

	}

	virtual bool requiresTextureFlipping() const
	{
		return false;
	}
};

static IDirect3DSurface9* curRT;

typedef float* (__cdecl * R_GetCodeMatrix_t)(char* contextA1, int matrixType, char matrixAdd);
R_GetCodeMatrix_t R_GetCodeMatrix = (R_GetCodeMatrix_t)0x52EB10;

void Inverse_DrawInvertified(GfxViewInfo* viewInfo, int a1, GfxCmdBufContext context)
{
	ogre_root->setNextFrameNumber(ogre_root->getNextFrameNumber()+1);

	IDirect3DStateBlock9* stateBlock;
	context.a2->device->CreateStateBlock(D3DSBT_ALL, &stateBlock);

	context.a2->device->GetRenderTarget(0, &curRT);
	context.a2->device->SetVertexShader(NULL);
	context.a2->device->SetPixelShader(NULL);

	RefVP vp(curRT);
	
	static bool didInit;
	static Ogre::Entity* ogreHead;
	static Ogre::SceneNode* ogreHeadNode;

	ogre_camera->setNearClipDistance(1.5f);
	ogre_camera->setFarClipDistance(3000.f);
	ogre_camera->setAspectRatio(1.666f);
	
	//float* viewProjection = (float*)(((char*)context.a1) + 1152 + 128);
	//float* viewProjection = R_GetCodeMatrix(context.a1, 86, 0);
	float* viewProjection = viewInfo->viewProjectionMatrix;
	//float viewProjection[16];
	float viewWorld[16];
	//context.a2->device->GetVertexShaderConstantF(0, (float*)&viewProjection, 4); // matrix 86 is equivalent to our projection/rotation matrix (view translation somehow is elsewhere)
	context.a2->device->GetVertexShaderConstantF(4, (float*)&viewWorld, 4);

	Ogre::Matrix4 ogreMatrix(viewProjection[0], viewProjection[1], viewProjection[2], viewProjection[3],
							 viewProjection[4], viewProjection[5], viewProjection[6], viewProjection[7],
							 viewProjection[8], viewProjection[9], viewProjection[10], viewProjection[11],
							 viewProjection[12], viewProjection[13], viewProjection[14], viewProjection[15]);

	ogreMatrix = ogreMatrix.transpose(); // as it comes from CodeMatrix it needs to be transposed to GL/shader-style format first

	/*Ogre::Matrix4 ogreMatrix2(viewWorld[0], viewWorld[1], viewWorld[2], viewWorld[3],
							  viewWorld[4], viewWorld[5], viewWorld[6], viewWorld[7],
							  viewWorld[8], viewWorld[9], viewWorld[10], viewWorld[11],
							  viewWorld[12], viewWorld[13], viewWorld[14], viewWorld[15]);*/

	Ogre::Matrix4 ogreMatrix2;
	ogreMatrix2.makeTrans(viewInfo->origin[0], viewInfo->origin[1], viewInfo->origin[2]);
	ogreMatrix2 = ogreMatrix2.inverse();

	//ogreMatrix2 = ogreMatrix2.transpose();
	
	ogre_camera->setCustomViewMatrix(true, ogreMatrix2);
	ogre_camera->setCustomProjectionMatrix(true, ogreMatrix); // our 'view' matrix is actually view/projection. hope this won't sting!

	if (!didInit)
	{
		Ogre::ResourceGroupManager& lRgMgr = Ogre::ResourceGroupManager::getSingleton();
		lRgMgr.createResourceGroup("g");
		lRgMgr.initialiseResourceGroup("g");
		lRgMgr.loadResourceGroup("g");

		std::string lFileName ="X:/MonsterHead.mesh"; 
		Ogre::MeshManager& lMeshManager = Ogre::MeshManager::getSingleton();	
		// I declare the mesh. You should create a manual resource loader to allow reloading (cf other tutorial).
		Ogre::MeshPtr lMesh = lMeshManager.createManual("m", "g");

		// Access/prepare the stream
		Ogre::FileSystemArchive filesystem("","FileSystem", true);
		bool readOnly = true;
		Ogre::DataStreamPtr lReadOnlyFile = filesystem.open(lFileName, readOnly);
		if(lReadOnlyFile.isNull())
		{
			throw std::exception("file not found!");
		}
		Ogre::DataStreamPtr lFile(OGRE_NEW Ogre::MemoryDataStream(lFileName, lReadOnlyFile));

		// Deserialize
		Ogre::MeshSerializer lFileReader;
		lFileReader.setListener(lMeshManager.getListener());
		lFileReader.importMesh(lFile, lMesh.get());

		ogreHead = ogre_sm->createEntity(lMesh);

		ogreHeadNode = ogre_sm->getRootSceneNode()->createChildSceneNode();
		ogreHeadNode->attachObject(ogreHead);

		didInit = true;
	}

	Ogre::Viewport* vvp = OGRE_NEW Ogre::Viewport(ogre_camera, &vp, 0, 0, 1440, 900, 0);

	vvp->setClearEveryFrame(false);
	vvp->update();

	OGRE_DELETE vvp;

	stateBlock->Apply();
	stateBlock->Release();
}
#else
#include "Inverse.h"

void Inverse_Init()
{

}

void Inverse_DrawInvertified(GfxViewInfo* viewInfo, int a1, GfxCmdBufContext context)
{

}
#endif