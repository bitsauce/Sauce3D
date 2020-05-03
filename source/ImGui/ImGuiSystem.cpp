//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#include <Sauce/Common.h>
#include <Sauce/Graphics.h>
#include <ImGui/ImGuiSystem.h>

BEGIN_SAUCE_NAMESPACE

ShaderRef g_ImGuiShader;
Texture2DRef g_FontTexture;
static ImGuiMouseCursor g_LastMouseCursor = ImGuiMouseCursor_COUNT;
static SDL_Cursor* g_ImGuiToSDLCursor[ImGuiMouseCursor_COUNT];

bool createShaders();
bool createFontsTexture();

void ImGuiSystem::initialize(void* hwnd)
{
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Set ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
	}

	{
        // Setup back-end capabilities flags
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
        io.BackendPlatformName = "imgui_impl_opengl4_sdl";

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
        io.KeyMap[ImGuiKey_Tab] = SAUCE_SCANCODE_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = SAUCE_SCANCODE_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = SAUCE_SCANCODE_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = SAUCE_SCANCODE_UP;
        io.KeyMap[ImGuiKey_DownArrow] = SAUCE_SCANCODE_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = SAUCE_SCANCODE_PAGEUP;
        io.KeyMap[ImGuiKey_PageDown] = SAUCE_SCANCODE_PAGEDOWN;
        io.KeyMap[ImGuiKey_Home] = SAUCE_SCANCODE_HOME;
        io.KeyMap[ImGuiKey_End] = SAUCE_SCANCODE_END;
        io.KeyMap[ImGuiKey_Insert] = SAUCE_SCANCODE_INSERT;
        io.KeyMap[ImGuiKey_Delete] = SAUCE_SCANCODE_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = SAUCE_SCANCODE_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = SAUCE_SCANCODE_SPACE;
        io.KeyMap[ImGuiKey_Enter] = SAUCE_SCANCODE_RETURN;
        io.KeyMap[ImGuiKey_Escape] = SAUCE_SCANCODE_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = SAUCE_SCANCODE_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = SAUCE_SCANCODE_A;
        io.KeyMap[ImGuiKey_C] = SAUCE_SCANCODE_C;
        io.KeyMap[ImGuiKey_V] = SAUCE_SCANCODE_V;
        io.KeyMap[ImGuiKey_X] = SAUCE_SCANCODE_X;
        io.KeyMap[ImGuiKey_Y] = SAUCE_SCANCODE_Y;
        io.KeyMap[ImGuiKey_Z] = SAUCE_SCANCODE_Z;

        g_ImGuiToSDLCursor[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        g_ImGuiToSDLCursor[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
        g_ImGuiToSDLCursor[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
        g_ImGuiToSDLCursor[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        g_ImGuiToSDLCursor[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        g_ImGuiToSDLCursor[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
        g_ImGuiToSDLCursor[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
        g_ImGuiToSDLCursor[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        g_ImGuiToSDLCursor[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
        // TODO: SDL_FreeCursor

#ifdef SAUCE_COMPILE_WINDOWS
        io.ImeWindowHandle = hwnd;
#endif
	}

    {
        // Setup back-end capabilities flags
        ImGuiIO& io = ImGui::GetIO();
        io.BackendRendererName = "imgui_impl_opengl4_sdl";
//#if IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
//        if (g_GlVersion >= 3200)
            io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
//#endif
    }

    createShaders();
}

void ImGuiSystem::processInputs(const float deltaTime, const char textInputChar)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    Window* window = Game::Get()->getWindow();
    InputManager* input = Game::Get()->getInputManager();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2((float)window->getWidth(), (float)window->getHeight());

    // Set delta time
    io.DeltaTime = deltaTime;

    // Read keyboard modifiers inputs
    io.KeyCtrl = input->getButtonState(SAUCE_KEY_LCTRL) || input->getButtonState(SAUCE_KEY_RCTRL);
    io.KeyShift = input->getButtonState(SAUCE_KEY_LSHIFT) || input->getButtonState(SAUCE_KEY_RSHIFT);
    io.KeyAlt = input->getButtonState(SAUCE_KEY_LALT) || input->getButtonState(SAUCE_KEY_RALT);
    io.KeySuper = false;


    // Update key states
    {
        for (int32 scancode = 0; scancode < 512; ++scancode)
        {
            const int32 keycode = SDL_GetKeyFromScancode((SDL_Scancode)scancode);
            io.KeysDown[scancode] = input->getButtonState((Keycode)keycode);
        }
        io.AddInputCharacter(textInputChar);
    }

    // Update mouse state
    {
        io.MouseDown[0] = input->getButtonState(MouseButton::SAUCE_MOUSE_BUTTON_LEFT);
        io.MouseDown[1] = input->getButtonState(MouseButton::SAUCE_MOUSE_BUTTON_RIGHT);
        io.MouseDown[2] = input->getButtonState(MouseButton::SAUCE_MOUSE_BUTTON_MIDDLE);
        io.MouseDown[3] = input->getButtonState(MouseButton::SAUCE_MOUSE_BUTTON_X1);
        io.MouseDown[4] = input->getButtonState(MouseButton::SAUCE_MOUSE_BUTTON_X2);

        // TODO:
        //io.MouseWheel += input->getAxisValue()
    }

    // Update mouse pos
    {
        ImGuiIO& io = ImGui::GetIO();

        // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
        if (io.WantSetMousePos)
        {
            SDL_WarpMouseInWindow(window->getSDLHandle(), io.MousePos.x, io.MousePos.y);
        }

        // Set mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);
        io.MousePos = ImVec2(x, y);
    }

    // Update OS mouse cursor with the cursor requested by imgui
    ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
    if (g_LastMouseCursor != mouse_cursor)
    {
        g_LastMouseCursor = mouse_cursor;

        ImGuiIO& io = ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
        {
            ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
            if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
            {
                // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
                SDL_ShowCursor(SDL_DISABLE);
            }
            else
            {
                // Show OS mouse cursor
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetCursor(g_ImGuiToSDLCursor[imgui_cursor]);
            }
        }
    }

    //ImGui_ImplGlfw_UpdateMousePosAndButtons();
    //ImGui_ImplGlfw_UpdateMouseCursor();

    // Update game controllers (if enabled and available)
    //ImGui_ImplGlfw_UpdateGamepads();
}

void ImGuiSystem::newFrame()
{
    ImGui::NewFrame();
}

void ImGuiSystem::render()
{
    ImGui::Render();

    GraphicsContext* graphicsContext = Game::Get()->getWindow()->getGraphicsContext();

    graphicsContext->setShader(g_ImGuiShader);

    ImDrawData* imDrawData = ImGui::GetDrawData();
    {
        VertexFormat fmt;
        fmt.set(VertexAttribute::VERTEX_POSITION, 2, Datatype::Float);
        fmt.set(VertexAttribute::VERTEX_COLOR, 4, Datatype::Uint8);
        fmt.set(VertexAttribute::VERTEX_TEX_COORD, 2, Datatype::Float);

        // Setup desired GL state
        // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
        // The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
//        GLuint vertex_array_object = 0;
//#ifndef IMGUI_IMPL_OPENGL_ES2
//        glGenVertexArrays(1, &vertex_array_object);
//#endif
//        ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = imDrawData->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clip_scale = imDrawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        // Render command lists
        for (int n = 0; n < imDrawData->CmdListsCount; n++)
        {
            const ImDrawList* imDrawList = imDrawData->CmdLists[n];

            // Convert ImGui vertex buffer to Sauce vertex buffer
            Vertex* vertices;
            uint32 vertexCount;
            {
                const ImVector<ImDrawVert>& imVbo = imDrawList->VtxBuffer;
                vertexCount = imVbo.Size;
                vertices = fmt.createVertices(vertexCount);
                for (uint32 i = 0; i < vertexCount; ++i)
                {
                    const uint8* col = (const uint8*)&imVbo[i].col;
                    vertices[i].set2f(VertexAttribute::VERTEX_POSITION, imVbo[i].pos.x, imVbo[i].pos.y);
                    vertices[i].set4ub(VertexAttribute::VERTEX_COLOR, col[0], col[1], col[2], col[3]);
                    vertices[i].set2f(VertexAttribute::VERTEX_TEX_COORD, imVbo[i].uv.x, imVbo[i].uv.y);
                }
            }

            // Convert ImGui index buffer to Sauce index buffer
            uint32* indices;
            uint32 indexCount;
            {
                const ImVector<ImDrawIdx>& imIbo = imDrawList->IdxBuffer;
                indexCount = imIbo.Size;
                indices = new uint32[indexCount];
                for (uint32 i = 0; i < indexCount; ++i)
                {
                    indices[i] = imIbo[i];
                }
            }

            for (int cmd_i = 0; cmd_i < imDrawList->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &imDrawList->CmdBuffer[cmd_i];
                if (pcmd->UserCallback != NULL)
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    // TODO: add callback
                    if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        assert(0);//ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                    else
                        pcmd->UserCallback(imDrawList, pcmd);
                }
                else
                {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec4 clip_rect;
                    clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                    clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                    clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                    clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                    // TODO: Fix clipping
//                    if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
//                    {
//                        // Apply scissor/clipping rectangle
//                        if (clip_origin_lower_left)
//                            glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
//                        else
//                            glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z, (int)clip_rect.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)
//
//                        // Bind texture, Draw
//                        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
//#if IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
//                        if (g_GlVersion >= 3200)
//                            glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
//                        else
//#endif
//                            glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
//                    }

                    g_ImGuiShader->setSampler2D("u_Texture", *(Texture2DRef*)pcmd->TextureId);
                    graphicsContext->drawIndexedPrimitives(PrimitiveType::PRIMITIVE_TRIANGLES, vertices, vertexCount, indices, indexCount);
                }
            }

            delete[] vertices;
            delete[] indices;
        }
    }

    graphicsContext->setShader(nullptr);
}

bool createShaders()
{
    const string vertexShader =
        "\n"
        "in vec2 in_Position;\n"
        "in vec2 in_TexCoord;\n"
        "in vec4 in_VertexColor;\n"
        "\n"
        "out vec2 v_TexCoord;\n"
        "out vec4 v_VertexColor;\n"
        "\n"
        "uniform mat4 u_ModelViewProj;\n"
        "\n"
        "void main()\n"
        "{\n"
        "	gl_Position = vec4(in_Position, 0.0, 1.0) * u_ModelViewProj;\n"
        "	v_TexCoord = in_TexCoord;\n"
        "	v_VertexColor = in_VertexColor;\n"
        "}\n";

    const string pixelShader =
        "\n"
        "in vec2 v_TexCoord;\n"
        "in vec4 v_VertexColor;\n"
        "\n"
        "out vec4 out_FragColor;\n"
        "\n"
        "uniform sampler2D u_Texture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "	out_FragColor = texture(u_Texture, v_TexCoord) * v_VertexColor;\n"
        "}\n";

    ShaderDesc shaderDesc;
    shaderDesc.shaderSourceVS = vertexShader;
    shaderDesc.shaderSourcePS = pixelShader;
    g_ImGuiShader = CreateNew<Shader>(shaderDesc);

    createFontsTexture();

    return true;
}

bool createFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int32 width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    Texture2DDesc textureDesc;
    Pixmap pixmap(width, height, PixelFormat(PixelComponents::RGBA, PixelDatatype::UNSIGNED_BYTE), pixels);
    textureDesc.pixmap = &pixmap;
    g_FontTexture = CreateNew<Texture2D>(textureDesc);

    // Store our identifier
    io.Fonts->TexID = (ImTextureID)&g_FontTexture;

    return true;
}

END_SAUCE_NAMESPACE

