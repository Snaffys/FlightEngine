#include <Window.h>
Window* Window::windowInstance = nullptr;

Window::Window(unsigned int width, unsigned int height) {
    window = glfwCreateWindow(width, height, "FlightSim", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Falied to initialize GLAD");
        exit(1);
    }

    this->width = width;
    this->height = height;

    windowInstance = this;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    lastX = width / 2.0f;
    lastY = height / 2.0f;

    camera = Camera(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f), 100.0f);

    //camera = Camera(glm::vec3(67.0f, 627.5f, 169.9f), glm::vec3(0.0f, 0.0f, -1.0f),
    //glm::vec3(0.0f, 1.0f, 0.0f), 1000.0f);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // SHADERS
    terrainShader = Shader("shaders/vertex/test.vert", "shaders/fragment/text.frag");
    postProcShader = Shader("shaders/vertex/postProc.vert", "shaders/fragment/postProc.frag");
    cubemapShader = Shader("shaders/vertex/cubemap.vert", "shaders/fragment/cubemap.frag");
    planeShader = Shader("shaders/vertex/plane.vert", "shaders/fragment/plane.frag");

    // TEXTURES
    char faces[][100] = {
        "res/cubemaps/right.jpg",
        "res/cubemaps/left.jpg",
        "res/cubemaps/top.jpg",
        "res/cubemaps/bottom.jpg",
        "res/cubemaps/front.jpg",
        "res/cubemaps/back.jpg"
    };
    cubemapTex = { sizeof(faces) / sizeof(faces)[0], sizeof(faces[0]) / sizeof(faces[0])[0], (char*)faces };

    // VBO/VAO/IBO
    postProcVbo = VBO({
        // vertex		    // texCoords
        {-1.0f,  1.0f,		0.0f, 1.0f},
        { 1.0f, -1.0f,		1.0f, 0.0f},
        {-1.0f, -1.0f,		0.0f, 0.0f},
        {-1.0f,  1.0f,		0.0f, 1.0f},
        { 1.0f,  1.0f,		1.0f, 1.0f},
        { 1.0f, -1.0f,		1.0f, 0.0f}
        });
    postProcVao = PosTex2DVao(postProcVbo);

    planeVbo = VBO({
        {-0.3f, -0.1f, -0.9f},
        {0.0f, 0.3f, -0.9f},
        {0.3f, -0.1f, -0.9f}
        });
    planeVao = Pos3DVao(planeVbo);

    cubemapVbo = {
        // BACK
        {-1.0f,  1.0f, -1.0f},  // top left
        { 1.0f,  1.0f, -1.0f},  // top right
        { 1.0f, -1.0f, -1.0f},  // bottom right
        { 1.0f, -1.0f, -1.0f},  // bottom right
        {-1.0f, -1.0f, -1.0f},  // bottom left
        {-1.0f,  1.0f, -1.0f},  // top left
        // RIGHT
        { 1.0f,  1.0f, -1.0f},  // top left
        { 1.0f,  1.0f,  1.0f},  // top right
        { 1.0f, -1.0f,  1.0f},  // bottom right
        { 1.0f, -1.0f,  1.0f},  // bottom right
        { 1.0f, -1.0f, -1.0f},  // bottom left
        { 1.0f,  1.0f, -1.0f},  // top left
        // LEFT
        {-1.0f,  1.0f,  1.0f},   // top left
        {-1.0f,  1.0f, -1.0f},   // top right
        {-1.0f, -1.0f, -1.0f},   // bottom right
        {-1.0f, -1.0f, -1.0f},   // bottom right
        {-1.0f, -1.0f,  1.0f},   // bottom left
        {-1.0f,  1.0f,  1.0f},   // top left
        // FRONT
        {-1.0f, -1.0f,  1.0f},  // bottom left
        { 1.0f, -1.0f,  1.0f},  // bottom right
        { 1.0f,  1.0f,  1.0f},  // top right
        { 1.0f,  1.0f,  1.0f},  // top right
        {-1.0f,  1.0f,  1.0f},  // top left
        {-1.0f, -1.0f,  1.0f},  // bottom left
        // TOP
        {-1.0f,  1.0f,  1.0f},  // top left
        { 1.0f,  1.0f,  1.0f},  // top right
        { 1.0f,  1.0f, -1.0f},  // bottom right
        { 1.0f,  1.0f, -1.0f},  // bottom right
        {-1.0f,  1.0f, -1.0f},  // bottom left
        {-1.0f,  1.0f,  1.0f},  // top left
        // BOTTOM
        {-1.0f, -1.0f, -1.0f},  // top left
        { 1.0f, -1.0f, -1.0f},  // top right
        { 1.0f, -1.0f,  1.0f},  // bottom right
        { 1.0f, -1.0f,  1.0f},  // bottom right
        {-1.0f, -1.0f,  1.0f},  // bottom left
        {-1.0f, -1.0f, -1.0f}   // top left
    };
    cubemapVao = Pos3DVao(cubemapVbo);

    // FBO
    postProcFbo = FBO(width, height, postProcTex);

    srand(time(NULL));
    // perlin noise seed creation 2D
    fNoiseSeed2D = new float[vertexWidthAmount * vertexHeightAmount];
    fPerlinNoise2D = new float[vertexWidthAmount * vertexHeightAmount];
    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount; i++)
        fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;	// random values [0.0, 1.0]

    // perlin noise seed creation 3D
    fNoiseSeed3D = new float[vertexWidthAmount * vertexHeightAmount * nOutputDepth];
    fPerlinNoise3D = new float[vertexWidthAmount * vertexHeightAmount * nOutputDepth];
    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount * nOutputDepth; i++)
        fNoiseSeed3D[i] = (float)rand() / (float)RAND_MAX;	// random values [0.0, 1.0]
}

void Window::launch() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);

    unsigned int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
    int vertsPerLine = (vertexWidthAmount - 1) / meshSimplificationIncrement + 1;
    const unsigned int numStrips = (vertexHeightAmount - 1) / meshSimplificationIncrement;     
    const unsigned int numVertsPerStrip = vertsPerLine * 2;  // each strip consitst of squares each square is 2 triangle (2 lines)
    
    try {
        while (!glfwWindowShouldClose(window)) {
            // for proper movement for every PC
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            postProcFbo.BindFramebuffer();
            glEnable(GL_DEPTH_TEST);

            glClearColor(0.1f, 0.3f, 0.6f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            terrainShader.Activate();

            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / height, 0.1f, 10000000.0f);
            terrainShader.setMat4("projection", projection);
            glm::mat4 view = camera.GetViewMatrix();
            terrainShader.setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            //model = glm::scale(model, glm::vec3(1000.0f, 500.0f, 1000.0f));
           // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            terrainShader.setMat4("model", model);

            if (polygonMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glDisable(GL_CULL_FACE);


            UpdateVisibleChunks();


            //auto start = std::chrono::high_resolution_clock::now();

            updateThread();

            // Visible chunks rendering
            for (Chunk& chunk : lastVisibleChunks) {

                glm::vec3 chunkCenter(chunk.position.x + vertexWidthAmount, 0.0f, chunk.position.y + vertexHeightAmount);
                float distanceToChunk = glm::distance(camera.GetCameraPosition(), chunkCenter);

                glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

                model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk.position.x * vertexWidthAmount, 0.0f, chunk.position.y * vertexHeightAmount));

                //model = glm::scale(model, glm::vec3(100.0f, 1.0f, 10.0f));

                terrainShader.setMat4("model", model);
                terrainShader.setFloat("posX", chunk.position.x);
                terrainShader.setFloat("posZ", chunk.position.y);

                if (chunk.terrainVaoC.IsValid()) {
                    chunk.terrainVaoC.Bind();
                    // render the mesh triangle strip by triangle strip - each row at a time
                    for (unsigned int strip = 0; strip < numStrips; ++strip)
                    {
                        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                            numVertsPerStrip, // number of indices to render
                            GL_UNSIGNED_INT,     // index data type
                            (void*)(sizeof(unsigned int)
                                * numVertsPerStrip
                                * strip)); // offset to starting index
                    }
                }
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glDisable(GL_CULL_FACE);


            // PLANE__________________________________________________________________________________________________________
            // planeShader.Activate();
            // //glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / height, 0.1f, 10000000.0f);
            // planeShader.setMat4("projection", projection);
            // //glm::mat4 view = camera.GetViewMatrix();
            // planeShader.setMat4("view", view);

            // //glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            // model = glm::scale(model, glm::vec3(1000.0f, 500.0f, 1000.0f));
            //// model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            // planeShader.setMat4("model", model);

            // planeVao.Bind();
            // glDrawArrays(GL_TRIANGLES, 0, 3);
            //__________________________________________________________________________________________________________________


            glEnable(GL_CULL_FACE);


            // CUBEMAP
            glDepthFunc(GL_LEQUAL); // depth test passes when values are equal or less than depth buffer's content
            cubemapShader.Activate();
            cubemapShader.setInt("cubemap", cubemapTex.GetId());
            glm::mat4 cubemapView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            cubemapShader.setMat4("projection", projection);
            cubemapShader.setMat4("view", cubemapView);
            cubemapVao.Bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);   // set depth function to default

            postProcFbo.UnbindFramebuffer();
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            // FRAMEBUFFER
            postProcShader.Activate();
            postProcVao.Bind();
            postProcShader.setInt("screenTexture", postProcTex.GetId());
            postProcShader.setFloat("exposure", exposure);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    catch (const char* errorMessage) {
        std::cout << errorMessage << std::endl;
    }
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    windowInstance->width = width;
    windowInstance->height = height;

    windowInstance->postProcTex.Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    windowInstance->postProcFbo.BindRenderbuffer();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}


void Window::mouseCallback(GLFWwindow* window, double xPos, double yPos) {

    if (windowInstance->firstMouse) {
        windowInstance->lastX = xPos;
        windowInstance->lastY = yPos;
        windowInstance->firstMouse = false;
    }

    float xOffset = xPos - windowInstance->lastX;
    float yOffset = windowInstance->lastY - yPos;
    windowInstance->lastX = xPos;
    windowInstance->lastY = yPos;

    windowInstance->camera.ProcessMouseMove(xOffset, yOffset);
}

void Window::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Process movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyPress(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyPress(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyPress(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyPress(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        polygonMode = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        polygonMode = true;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
    {
        octavePressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !octavePressed) {
        nOctaveCount++;
        octavePressed = true;
        if (nOctaveCount == 10 || vertexWidthAmount < pow(2.0f, nOctaveCount - 1))
            nOctaveCount = 1;

        UpdateVisibleChunks();


        printf("\noctave count:%u", nOctaveCount);

    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        for (unsigned int i = 0; i < vertexHeightAmount * vertexWidthAmount; i++)
            fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;	// random values [0.0, 1.0]

        UpdateVisibleChunks();
    }


    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        fScalingBias += 0.2;

        UpdateVisibleChunks();


        printf("\nscaling bias:%f", fScalingBias);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        fScalingBias -= 0.2;
        if (fScalingBias < 0.2)
            fScalingBias = 0.2;

        UpdateVisibleChunks();

        printf("\nscaling bias:%f", fScalingBias);
    }

}

void Window::PerlinNoise2D(int mapWidth, int mapHeight, float* fSeed, int nOctaves, float bias, float* mapOutput) {
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++)
        {
            float noise = 0.0f;
            float maxScale = 0.0f;

            float scale = 1.0f;    // determines how much lower the contribution for each octave is (makes higher octaves more significant effect)

            for (int oct = 0; oct < nOctaves; oct++)
            {
                int nPitchX = mapWidth >> oct;  // make octave more persistent every loop
                int nPitchY = mapHeight >> oct;

                int nSampleX1 = (x / nPitchX) * nPitchX;
                int nSampleX2 = (nSampleX1 + nPitchX) % mapWidth;  // '%' to make values wrap around

                int nSampleY1 = (y / nPitchY) * nPitchY;
                int nSampleY2 = (nSampleY1 + nPitchY) % mapHeight;

                float fBlendX = fade((float)(x - nSampleX1) / (float)nPitchX);
                float fBlendY = fade((float)(y - nSampleY1) / (float)nPitchY);
                float fSampleT = lerp(fSeed[nSampleY1 * mapWidth + nSampleX1], fSeed[nSampleY1 * mapWidth + nSampleX2], fBlendX);   // interpolate between 2 top values
                float fSampleB = lerp(fSeed[nSampleY2 * mapWidth + nSampleX1], fSeed[nSampleY2 * mapWidth + nSampleX2], fBlendX);   // interpolate between 2 bottom values

                maxScale += scale;    // maximum possible number for fitting output in range

                noise += lerp(fSampleT, fSampleB, fBlendY) * scale;   // interpolate between top and bottom interpolation results

                //scale *= scale / bias;
                scale /= bias;
            }

            mapOutput[y * mapWidth + x] = (noise / maxScale);   // make range of output between 0 and 1
            mapOutput[y * mapWidth + x] *= 256;
        }
    }
}

float Window::lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float Window::fade(float t) {
    //return t;
    return t * t * t * (t * (t * 6 - 15) + 10);
    //return t* t* (3 - 2 * t);
}

void Window::generateTerrain(Chunk& chunk) {
    for (unsigned int i = 0; i < vertexWidthAmount * vertexHeightAmount; i++)
        fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;	// random values [0.0, 1.0]

    PerlinNoise2D(vertexWidthAmount, vertexHeightAmount, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

    unsigned int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
    // -1 because last indexes of vertices start with 0
    int verticesPerLine = ((vertexWidthAmount - 1) / meshSimplificationIncrement) + 1;

    std::vector<float> vertices;
    float yShift = (vertexWidthAmount + vertexHeightAmount) / 2.0;
    for (unsigned int i = 0; i < vertexWidthAmount; i+=meshSimplificationIncrement) {
        for (unsigned int j = 0; j < vertexHeightAmount; j+=meshSimplificationIncrement) {
            float y = (float)((fPerlinNoise2D[j * vertexWidthAmount + i]));

            //chunk.vertices.push_back(-vertexWidthAmount * 2.0f + i);
            //chunk.vertices.push_back(y - 0);
            //chunk.vertices.push_back(-vertexHeightAmount * 2.0f + j);
            chunk.vertices.push_back(i);
            chunk.vertices.push_back(y - 0);
            chunk.vertices.push_back(j);
        }
    }

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < verticesPerLine - 1; i++)     // for each row (strip)
        for (unsigned int j = 0; j < verticesPerLine; j++)    // for each column
            for (unsigned int k = 0; k < 2; k++)            // for each side of the strip (leftand right primitives in square)
                chunk.indices.push_back(verticesPerLine * (i + k) + j);   // top left -> top right -> bottom left -> bottom right


    //chunk.terrainVboC = VBO(chunk.vertices);
    //chunk.terrainVaoC = TerrainVao(chunk.terrainVboC);
    //chunk.terrainIboC = IBO(chunk.indices);
}


void Window::UpdateVisibleChunks() {
    for (Chunk& chunk : lastVisibleChunks) {
        if(chunks.find(chunk.position) != chunks.end())
            chunk.loaded = false;
    }
    lastVisibleChunks.clear();
    
    int currentChunkCoordX = floor(camera.GetCameraPosition().x / chunkSize);
    int currentChunkCoordZ = floor(camera.GetCameraPosition().z / chunkSize);

    for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst - 1; yOffset++) {
        for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst - 1; xOffset++) {
            glm::vec2 viewedChunkCoord = glm::vec2(currentChunkCoordX + xOffset, currentChunkCoordZ + yOffset);
            
            if (chunks.find(viewedChunkCoord) != chunks.end()) {
                Chunk foundedChunk = chunks.at(viewedChunkCoord);
                updateChunk(foundedChunk, maxViewDst, currentChunkCoordX, currentChunkCoordZ);

                if (foundedChunk.loaded == true)
                    lastVisibleChunks.push_back(foundedChunk);
            }
            else {
                Chunk chunk;
                chunk.position = viewedChunkCoord;
                chunk.loaded = false;
                chunks.insert({ chunk.position, chunk });

                requestMapData(viewedChunkCoord, onMapDataReceived);
            }
        }
    }
}

void Window::updateChunk(Chunk& chunk, unsigned int maxViewDst, int chunkCoordX, int chunkCoordY) {
    float xPos = chunk.position.x * vertexWidthAmount + ((chunkCoordX < chunk.position.x) * vertexWidthAmount);
    float zPos = chunk.position.y * vertexHeightAmount + ((chunkCoordY < chunk.position.y) * vertexHeightAmount);

    bool visible = distance(glm::vec3(camera.GetCameraPosition().x, 0.0, camera.GetCameraPosition().z), glm::vec3(xPos, 0, zPos)) <= maxViewDst;
    chunk.loaded = visible;
}

void Window::requestMapData(glm::vec2 chunkPos, void (*callbackFunc)(Chunk&)) {
    std::thread thread([this, chunkPos, callbackFunc]() {
        mapDataThread(chunkPos, callbackFunc);
    });
    thread.detach();
}  


void Window::mapDataThread(glm::vec2 chunkPos, void (*callbackFunc)(Chunk&)) {
    Chunk chunk;
    generateTerrain(chunk);
    chunk.position = chunkPos;
    chunk.loaded = false;
    {
        queueMutex.lock();
        chunks[chunkPos] = chunk;
        mapDataThreadInfoQueue.push(MapThreadInfo(callbackFunc, chunks[chunkPos]));
        queueMutex.unlock();
    }
}

void Window::updateThread() {
    while (!mapDataThreadInfoQueue.empty()) {
        MapThreadInfo threadInfo = mapDataThreadInfoQueue.front();
        mapDataThreadInfoQueue.pop();
        threadInfo.callback(threadInfo.parameter);
    }
}

void Window::onMapDataReceived(Chunk& chunk) {
    windowInstance->chunks[chunk.position].terrainVboC = VBO(chunk.vertices);
    windowInstance->chunks[chunk.position].terrainVaoC = TerrainVao(chunk.terrainVboC);
    windowInstance->chunks[chunk.position].terrainIboC = IBO(chunk.indices);
}

void Window::LODMesh(int lod) {
    this->lod = lod;
}

void Window::RequestMesh(Chunk chunk) {
    this->hasRequestedMesh = true;
}

Window::~Window() {}