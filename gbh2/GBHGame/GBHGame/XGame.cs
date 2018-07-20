using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Input.Touch;
using Microsoft.Xna.Framework.Media;

namespace GBH
{
    /// <summary>
    /// This is the main type for your game - oh is it?
    /// </summary>
    public class XGame : Microsoft.Xna.Framework.Game
    {
        GraphicsDeviceManager graphics;

        public XGame()
        {
            Content.RootDirectory = "Content";

            graphics = new GraphicsDeviceManager(this);
            graphics.PreferredDepthStencilFormat = DepthFormat.Depth16;
            graphics.PreferredBackBufferWidth = 1280;
            graphics.PreferredBackBufferHeight = 720;

            TargetElapsedTime = TimeSpan.FromTicks(166666);
        }

        protected override void Initialize()
        {
            //EventInput.Initialize(this.Window);

            Log.Initialize(LogLevel.All);
            Log.AddListener(new ConsoleLogListener());
            Log.AddListener(new GameLogListener());

            ConVar.Initialize();
            FileSystem.Initialize();
            StyleManager.Load("Styles/bil.sty");
            MapManager.Load("Maps/MP1-comp.gmp");
            MapGeometry.Initialize();
            Camera.Initialize();

            base.Initialize();
        }

        protected override void LoadContent()
        {
            ConsoleRenderer.Initialize();
            StyleManager.CreateTextures(GraphicsDevice);
            MapRenderer.Initialize(GraphicsDevice);
            DeferredRenderer.Initialize(GraphicsDevice);
            Renderer2D.Initialize(GraphicsDevice);
        }

        protected override void UnloadContent()
        {
        }

        protected override void Update(GameTime gameTime)
        {
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
                this.Exit();

            Camera.Process();

            // camera moving
            float dT = (float)(gameTime.ElapsedGameTime.TotalMilliseconds / 1000);
            Vector3 delta = new Vector3();

            KeyboardState keyState = Keyboard.GetState();
            if (keyState.IsKeyDown(Keys.Right))
            {
                delta += new Vector3(3.0f * dT, 0, 0);
            }

            if (keyState.IsKeyDown(Keys.Left))
            {
                delta += new Vector3(-3.0f * dT, 0, 0);
            }

            if (keyState.IsKeyDown(Keys.Up))
            {
                delta += new Vector3(0, 3.0f * dT, 0);
            }

            if (keyState.IsKeyDown(Keys.Down))
            {
                delta += new Vector3(0, -3.0f * dT, 0);
            }

            if (keyState.IsKeyDown(Keys.PageUp))
            {
                delta += new Vector3(0, 0, -3.0f * dT);
            }

            if (keyState.IsKeyDown(Keys.PageDown))
            {
                delta += new Vector3(0, 0, 3.0f * dT);
            }

            Camera.MainCamera.Position += delta;

            if (Camera.MainCamera.Position.X > 255)
            {
                Camera.MainCamera.Position = new Vector3(4.0f, 40.0f, 12.0f);
            }

            Renderer2D.InitPerFrame();

            ConsoleRenderer.Process();

            base.Update(gameTime);
        }

        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(ClearOptions.DepthBuffer | ClearOptions.Target, Color.Black, 1.0f, 0);

            DeferredRenderer.Render3DStuff(GraphicsDevice);
            Renderer2D.Render(GraphicsDevice);

            base.Draw(gameTime);
        }
    }
}
