// based on code from http://mynameismjp.wordpress.com/2009/01/19/deferred-shadow-maps-sample/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;

namespace GBH
{
    public static class ShadowRenderer
    {
        private static Effect _shadowEffect;
        public static RenderTarget2D _shadowMap;
        public static RenderTarget2D _shadowOcclusion;
        private static RenderTarget2D _disabledShadowOcclusion;

        private static EffectTechnique[] shadowOcclusionTechniques = new EffectTechnique[4];

        private static ConVar sm_enable;
        private static ConVar sm_filterType;

        public static void Initialize(GraphicsDevice device)
        {
            _shadowEffect = EffectManager.Load("Shadow", device);

            _shadowMap = new RenderTarget2D(device, 2048, 2048, false, SurfaceFormat.Single, DepthFormat.Depth16);
            _shadowOcclusion = new RenderTarget2D(device, device.PresentationParameters.BackBufferWidth, device.PresentationParameters.BackBufferHeight, false, SurfaceFormat.Color, DepthFormat.None);
            _disabledShadowOcclusion = new RenderTarget2D(device, 1, 1, false, SurfaceFormat.Color, DepthFormat.None);

            shadowOcclusionTechniques[0] = _shadowEffect.Techniques["CreateShadowTerm2x2PCF"];
            /*shadowOcclusionTechniques[1] = _shadowEffect.Techniques["CreateShadowTerm3x3PCF"];
            shadowOcclusionTechniques[2] = _shadowEffect.Techniques["CreateShadowTerm5x5PCF"];
            shadowOcclusionTechniques[3] = _shadowEffect.Techniques["CreateShadowTerm7x7PCF"];*/

            _frustumCornersVS = new Vector3[8];
            _frustumCornersWS = new Vector3[8];
            _frustumCornersLS = new Vector3[8];
            _farFrustumCornersVS = new Vector3[4];

            sm_enable = ConVar.Register("sm_enable", true, "Enable shadow mapping", ConVarFlags.Archived);
            sm_filterType = ConVar.Register("sm_filterType", 0, "Defines the filtering algorithm to use for shadow mapping", ConVarFlags.Archived);
        }

        private static Vector3[] _frustumCornersLS;
        private static Vector3[] _frustumCornersWS;
        private static Vector3[] _frustumCornersVS;
        private static Vector3[] _farFrustumCornersVS;
        //private static OrthographicCamera _lightCamera;
        private static Camera _lightCamera;

        public static RenderTarget2D RenderShadows(GraphicsDevice device)
        {
            if (sm_enable.GetValue<bool>())
            {
                Matrix cameraTransform, viewMatrix;
                Camera.MainCamera.GetWorldMatrix(out cameraTransform);
                Camera.MainCamera.GetViewMatrix(out viewMatrix);
                Camera.MainCamera.BoundingFrustum.GetCorners(_frustumCornersWS);
                Vector3.Transform(_frustumCornersWS, ref viewMatrix, _frustumCornersVS);

                for (int i = 0; i < 4; i++)
                {
                    _farFrustumCornersVS[i] = _frustumCornersVS[i + 4];
                }

                CalculateFrustum();

                RenderShadowMap(device);

                //return null;

                RenderShadowOcclusion(device, DeferredRenderer.DepthRT);

                return _shadowOcclusion;
            }
            else
            {
                var renderTargets = device.GetRenderTargets();
                device.SetRenderTarget(_disabledShadowOcclusion);
                device.Clear(ClearOptions.Target, Color.White, 1.0f, 0);
                device.SetRenderTargets(renderTargets);

                return _disabledShadowOcclusion;
            }
        }

        private static void CalculateFrustum()
		{            
			// Shorten the view frustum according to the shadow view distance
			Matrix cameraMatrix;
			Camera.MainCamera.GetWorldMatrix(out cameraMatrix);

			// Find the centroid
            Vector3 frustumCentroid = new Vector3(0, 0, 0);
			for (int i = 0; i < 8; i++)
            {
				frustumCentroid += _frustumCornersWS[i];
            }

			frustumCentroid /= 8;

			// Position the shadow-caster camera so that it's looking at the centroid,
			// and backed up in the direction of the sunlight
            float distFromCentroid = MathHelper.Max((Camera.MainCamera.FarClip - Camera.MainCamera.NearClip), Vector3.Distance(_frustumCornersVS[4], _frustumCornersVS[5])) + 50.0f;
            //float distFromCentroid = 10.0f;
            Matrix viewMatrix = Matrix.CreateLookAt(frustumCentroid - (new Vector3(0.5f, 0.5f, -1.0f) * distFromCentroid), frustumCentroid, new Vector3(0, 0, 1));

			// Determine the position of the frustum corners in light space
			Vector3.Transform(_frustumCornersWS, ref viewMatrix, _frustumCornersLS);

			// Calculate an orthographic projection by sizing a bounding box 
			// to the frustum coordinates in light space
			Vector3 mins = _frustumCornersLS[0];
			Vector3 maxes = _frustumCornersLS[0];
			for (int i = 0; i < 8; i++)
			{
				if (_frustumCornersLS[i].X > maxes.X)
					maxes.X = _frustumCornersLS[i].X;
				else if (_frustumCornersLS[i].X < mins.X)
					mins.X = _frustumCornersLS[i].X;
				if (_frustumCornersLS[i].Y > maxes.Y)
					maxes.Y = _frustumCornersLS[i].Y;
				else if (_frustumCornersLS[i].Y < mins.Y)
					mins.Y = _frustumCornersLS[i].Y;
				if (_frustumCornersLS[i].Z > maxes.Z)
					maxes.Z = _frustumCornersLS[i].Z;
				else if (_frustumCornersLS[i].Z < mins.Z)
					mins.Z = _frustumCornersLS[i].Z;
			}     

            // Create an orthographic camera for use as a shadow caster
            //const float nearClipOffset = 100.0f;
            const float nearClipOffset = 20.0f;
            _lightCamera = new OrthographicCamera(mins.X, maxes.X, mins.Y, maxes.Y, -maxes.Z - nearClipOffset, -mins.Z);
            _lightCamera.SetViewMatrix(ref viewMatrix);

            //_lightCamera = new PerspectiveCamera(MathHelper.ToRadians(30f), 1f, 0.1f, 40f);
            //_lightCamera.SetViewMatrix(ref viewMatrix);
		}

        private static void RenderShadowMap(GraphicsDevice graphicsDevice)
        {
            // Set the shadow map as the current render target and clear it
            graphicsDevice.SetRenderTargets(_shadowMap);
            graphicsDevice.Clear(ClearOptions.Target, new Vector4(1.0f), 1.0f, 0);
            graphicsDevice.Clear(ClearOptions.DepthBuffer, new Vector4(1.0f), 1.0f, 0);

            // Set up the effect
            _shadowEffect.CurrentTechnique = _shadowEffect.Techniques["GenerateShadowMap"];
            //_shadowEffect.Parameters["g_matWorld"].SetValue(Matrix.Identity);
            _shadowEffect.Parameters["g_matViewProj"].SetValue(_lightCamera.ViewProjectionMatrix);

            _shadowEffect.CurrentTechnique.Passes[0].Apply();

            DeferredRenderer.RenderScene(graphicsDevice, _shadowEffect, _lightCamera);
        }

        private static void RenderShadowOcclusion(GraphicsDevice graphicsDevice, RenderTarget2D depthTexture)
        {
            // Set the device to render to our shadow occlusion texture, and to use
            // the original DepthStencilSurface
            graphicsDevice.SetRenderTarget(_shadowOcclusion);

            Matrix cameraTransform;
            Camera.MainCamera.GetWorldMatrix(out cameraTransform);

            // Setup the Effect
            _shadowEffect.CurrentTechnique = shadowOcclusionTechniques[sm_filterType.GetValue<int>()];
            _shadowEffect.Parameters["InvertViewProjection"].SetValue(Matrix.Invert(Camera.MainCamera.ViewProjectionMatrix));
            _shadowEffect.Parameters["g_matInvView"].SetValue(cameraTransform);
            _shadowEffect.Parameters["g_matLightViewProj"].SetValue(_lightCamera.ViewProjectionMatrix);
            _shadowEffect.Parameters["g_vFrustumCornersVS"].SetValue(_farFrustumCornersVS);
            _shadowEffect.Parameters["ShadowMap"].SetValue(_shadowMap);
            _shadowEffect.Parameters["DepthTexture"].SetValue(depthTexture);
            _shadowEffect.Parameters["g_vOcclusionTextureSize"].SetValue(new Vector2(_shadowOcclusion.Width, _shadowOcclusion.Height));
            _shadowEffect.Parameters["g_vShadowMapSize"].SetValue(new Vector2(2048, 2048));

            // Begin effect
            _shadowEffect.CurrentTechnique.Passes[0].Apply();

            // Draw the full screen quad		
            DeferredRenderer.RenderFullScreenQuad(graphicsDevice);
        }
    }
}
