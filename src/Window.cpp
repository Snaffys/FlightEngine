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
//    glm::vec3(0.0f, 1.0f, 0.0f), 1000.0f);

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

    //terrainVbo = VBO({
    //    { 0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f,0.0f},  // top right
    //    { 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,0.0f},  // bottom right 
    //    {-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,0.0f},  // bottom left 
    //    {-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,0.0f}   // top left
    //    });
    //terrainVao = VAO(terrainVbo);
    //// always bind IBO after VAO
    //terrainIbo = IBO({
    //    { 0,1,3 },
    //    { 1,2,3 }
    //    });

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
    // set size
    fNoiseSeed2D = new float[nOutputWidth * nOutputHeight];
    fPerlinNoise2D = new float[nOutputWidth * nOutputHeight];

    for (unsigned int i = 0; i < nOutputWidth * nOutputHeight; i++)
        fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;	// random values [0.0, 1.0]
}

void Window::launch() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);

    unsigned int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
    int ver = (nOutputWidth - 1) / meshSimplificationIncrement + 1;

    const unsigned int NUM_STRIPS = (nOutputHeight - 1) / meshSimplificationIncrement;     
    const unsigned int NUM_VERTS_PER_STRIP = ver * 2;


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

        for (Chunk& chunk : lastVisibleChunks) {

            glm::vec3 chunkCenter(chunk.position.x + nOutputWidth, 0.0f, chunk.position.y + nOutputHeight);
            float distanceToChunk = glm::distance(camera.GetCameraPosition(), chunkCenter);

                glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

                model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk.position.x * nOutputWidth, 0.0f, chunk.position.y * nOutputHeight));

                //model = glm::scale(model, glm::vec3(100.0f, 1.0f, 10.0f));

                terrainShader.setMat4("model", model);
                terrainShader.setFloat("posX", chunk.position.x);
                terrainShader.setFloat("posZ", chunk.position.y);


                chunk.terrainVaoC.Bind();
                // render the mesh triangle strip by triangle strip - each row at a time
                for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
                {
                    glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                        NUM_VERTS_PER_STRIP, // number of indices to render
                        GL_UNSIGNED_INT,     // index data type
                        (void*)(sizeof(unsigned int)
                            * NUM_VERTS_PER_STRIP
                            * strip)); // offset to starting index
                } 
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDisable(GL_CULL_FACE);

        planeShader.Activate();

        //glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / height, 0.1f, 10000000.0f);
        planeShader.setMat4("projection", projection);
        //glm::mat4 view = camera.GetViewMatrix();
        planeShader.setMat4("view", view);

        //glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::scale(model, glm::vec3(1000.0f, 500.0f, 1000.0f));
       // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        planeShader.setMat4("model", model);

        planeVao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

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


        //printf("%d \n", chunks.size());
    }
}

//void Window::RequestMapData() {
//
//}
//
//void Window::MapDataThread() {
//
//}

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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyPress(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyPress(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyPress(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyPress(LEFT, deltaTime);

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
        if (nOctaveCount == 10 || nOutputWidth < pow(2.0f, nOctaveCount - 1))
            nOctaveCount = 1;

        chunks.clear();
        UpdateVisibleChunks();


        printf("\noctave count:%u", nOctaveCount);

    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        for (unsigned int i = 0; i < nOutputHeight * nOutputWidth; i++)
            fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;	// random values [0.0, 1.0]

        chunks.clear(); 
        UpdateVisibleChunks();
    }


    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        fScalingBias += 0.2;

        chunks.clear(); 
        UpdateVisibleChunks();


        printf("\nscaling bias:%f", fScalingBias);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        fScalingBias -= 0.2;
        if (fScalingBias < 0.2)
            fScalingBias = 0.2;

        chunks.clear(); 
        UpdateVisibleChunks();

        printf("\nscaling bias:%f", fScalingBias);
    }

}

void Window::test() {

    //float maxNoiseHeight = 1.0f;
    //float minNoiseHeight = 0.0f;
    //
    //float scale = 1;

    //for (int x = 0; x < nOutputWidth; x++) {
    //    for (int y = 0; y < nOutputHeight; y++)
    //    {
    //        float amplitude = 1;
    //        float frequency = 1;
    //        float noiseHeight = 0;
    //        float persistance = 1;
    //        float lacunarity = 1;

    //        for (int i = 0; i < nOctaveCount; i++) {
    //            float sampleX = x / scale * frequency;
    //            float sampleY = y / scale * frequency;

    //            float perlinValue = 0;

    //            noiseHeight += perlinValue * amplitude;

    //            amplitude *= persistance;
    //            frequency *= lacunarity;
    //        }

    //        if (noiseHeight > maxNoiseHeight)
    //            maxNoiseHeight = noiseHeight;
    //        else if (noiseHeight < minNoiseHeight)
    //            minNoiseHeight = noiseHeight;

    //        fPerlinNoise2D[y * nOutputWidth + x] = noiseHeight;   // make range of output between 0 and 1
    //        fPerlinNoise2D[y * nOutputWidth + x] *= 256;

    //    }
    //}
}

void Window::PerlinNoise2D(int mapWidth, int mapHeight, float* fSeed, int nOctaves, float bias, float* mapOutput) {
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++)
        {
            float noise = 0.0f;
            float maxScale = 0.0f;

            float scale = 1.0f;    // determines how much lower the contribution for each octave is (makes higher octaves more significant effect)

            for (int o = 0; o < nOctaves; o++)
            {
                int nPitchX = mapWidth >> o;  // make octave more persistent every loop
                int nPitchY = mapHeight >> o;

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


void Window::PerlinNoise3D(int mapWidth, int mapHeight, int mapDepth, float* fSeed, int nOctaves, float bias, float* mapOutput) {

    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            for (int z = 0; z < mapDepth; z++) {
                float noise = 0.0f;
                float maxScale = 0.0f;
                float scale = 1.0f;
                for (int oct = 0; oct < nOctaves; oct++) {

                    int nPitchX = mapWidth >> oct;
                    int nPitchY = mapHeight >> oct;
                    int nPitchZ = mapDepth >> oct;

                    int nSampleX1 = (x / nPitchX) * nPitchX;
                    int nSampleX2 = (nSampleX1 + nPitchX) % mapWidth;  // '%' to make values wrap around

                    int nSampleY1 = (y / nPitchY) * nPitchY;
                    int nSampleY2 = (nSampleY1 + nPitchY) % mapHeight;

                    int nSampleZ1 = (z / nPitchZ) * nPitchZ;
                    int nSampleZ2 = (nSampleZ1 + nPitchZ) % mapDepth;

                    float fBlendX = fade((float)(x - nSampleX1) / (float)nPitchX);
                    float fBlendY = fade((float)(y - nSampleY1) / (float)nPitchY);
                    float fBlendZ = fade((float)(z - nSampleZ1) / (float)nPitchZ);

                    float fSampleTF = lerp(fSeed[nSampleY1 * mapWidth + nSampleZ1 * (mapWidth * mapHeight) + nSampleX1], fSeed[nSampleY1 * mapWidth + nSampleZ1 * (mapWidth * mapHeight) + nSampleX2], fBlendX);   // interpolate between 2 top values
                    float fSampleTB = lerp(fSeed[nSampleY1 * mapWidth + nSampleZ2 * (mapWidth * mapHeight) + nSampleX1], fSeed[nSampleY1 * mapWidth + nSampleZ2 * (mapWidth * mapHeight) + nSampleX2], fBlendX);   // interpolate between 2 top values
                    float fSampleBF = lerp(fSeed[nSampleY2 * mapWidth + nSampleZ1 * (mapWidth * mapHeight) + nSampleX1], fSeed[nSampleY2 * mapWidth + nSampleZ1 * (mapWidth * mapHeight) + nSampleX2], fBlendX);   // interpolate between 2 bottom values
                    float fSampleBB = lerp(fSeed[nSampleY2 * mapWidth + nSampleZ2 * (mapWidth * mapHeight) + nSampleX1], fSeed[nSampleY2 * mapWidth + nSampleZ2 * (mapWidth * mapHeight) + nSampleX2], fBlendX);   // interpolate between 2 bottom values

                    maxScale += scale;    // maximum possible number for fitting output in range

                    float fSampleTop = lerp(fSampleTF, fSampleTB, fBlendZ);
                    float fSampleBottom = lerp(fSampleBF, fSampleBB, fBlendZ);

                    noise += lerp(fSampleTop, fSampleBottom, fBlendY) * scale;   // interpolate between top and bottom interpolation results

                    //scale *= scale / bias;
                    scale /= bias;
                }

                mapOutput[y * mapWidth + z * (mapWidth * mapHeight) + x] = (noise / maxScale);   // make range of output between 0 and 1
                mapOutput[y * mapWidth + z * (mapWidth * mapHeight) + x] *= 256;
            }
        }
    }
}

void Window::generateTerrain3D(Chunk& chunk) {
    PerlinNoise3D(nOutputWidth, nOutputHeight, nOutputDepth, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

    unsigned int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
    int verticesPerLine = (nOutputWidth - 1) / meshSimplificationIncrement + 1;



    std::vector<float> vertices;


    for (unsigned int i = 0; i < nOutputWidth; i += meshSimplificationIncrement) {
        for (unsigned int j = 0; j < nOutputHeight; j += meshSimplificationIncrement) {
            for (unsigned int k = 0; k < nOutputDepth; k += meshSimplificationIncrement) {
                float y = (float)((fPerlinNoise2D[j * nOutputWidth + i]));

                //chunk.vertices.push_back(-nOutputWidth * 2.0f + i);
                //chunk.vertices.push_back(y - 0);
                //chunk.vertices.push_back(-nOutputHeight * 2.0f + j);
                chunk.vertices.push_back(i);
                chunk.vertices.push_back(y - 0);
                chunk.vertices.push_back(j);
            }
        }
    }

    std::vector<unsigned int> indices;


    for (unsigned int i = 0; i < verticesPerLine - 1; i++)     // for each row (strip)
        for (unsigned int j = 0; j < verticesPerLine; j++)    // for each column
            for (unsigned int k = 0; k < 2; k++)            // for each side of the strip (leftand right primitives in square)
                chunk.indices.push_back(verticesPerLine * (i + k) + j);   // top left -> top right -> bottom left -> bottom right




    chunk.terrainVboC = VBO(chunk.vertices);
    chunk.terrainVaoC = TerrainVao(chunk.terrainVboC);
    chunk.terrainIboC = IBO(chunk.indices);
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
    PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

    unsigned int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
    int verticesPerLine = (nOutputWidth - 1) / meshSimplificationIncrement + 1;

    std::vector<float> vertices;
    float yShift = (nOutputWidth + nOutputHeight) / 2.0;
    for (unsigned int i = 0; i < nOutputWidth; i+=meshSimplificationIncrement) {
        for (unsigned int j = 0; j < nOutputHeight; j+=meshSimplificationIncrement) {
            float y = (float)((fPerlinNoise2D[j * nOutputWidth + i]));

            //chunk.vertices.push_back(-nOutputWidth * 2.0f + i);
            //chunk.vertices.push_back(y - 0);
            //chunk.vertices.push_back(-nOutputHeight * 2.0f + j);
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

    chunk.terrainVboC = VBO(chunk.vertices);
    chunk.terrainVaoC = TerrainVao(chunk.terrainVboC);
    chunk.terrainIboC = IBO(chunk.indices);
}

void Window::updateChunk(Chunk& chunk, unsigned int maxViewDst, int chunkCoordX, int chunkCoordY) {
    float xPos = chunk.position.x * nOutputWidth + ((chunkCoordX < chunk.position.x) * nOutputWidth);
    float zPos = chunk.position.y * nOutputHeight + ((chunkCoordY < chunk.position.y) * nOutputHeight);

    bool visible = distance(glm::vec3(camera.GetCameraPosition().x, 0.0, camera.GetCameraPosition().z), glm::vec3(xPos, 0, zPos)) <= maxViewDst;
    chunk.loaded = visible;
}

void Window::UpdateVisibleChunks() {

    for (Chunk& chunk : lastVisibleChunks)
        chunk.loaded = false;
    lastVisibleChunks.clear();
    
    int currentChunkCoordX = floor(camera.GetCameraPosition().x / chunkSize);
    int currentChunkCoordY = floor(camera.GetCameraPosition().z / chunkSize);

    bool chunkNotGenerated = true;

    for (int yOffset = -chunksVisibleInViewDst; yOffset <= chunksVisibleInViewDst; yOffset++) {
        for (int xOffset = -chunksVisibleInViewDst; xOffset <= chunksVisibleInViewDst; xOffset++) {
            glm::vec2 viewedChunkCoord = glm::vec2(currentChunkCoordX + xOffset, currentChunkCoordY + yOffset);
            for (Chunk& chunk : chunks)
            {
                if (chunk.position == viewedChunkCoord) {
                    updateChunk(chunk, maxViewDst, currentChunkCoordX, currentChunkCoordY);

                    if (chunk.loaded == true)
                        lastVisibleChunks.push_back(chunk);

                    chunkNotGenerated = false;
                    break;
                }
            }
            if (chunkNotGenerated) {
                Chunk chunk;
                generateTerrain(chunk);

                chunk.position = viewedChunkCoord;
                chunk.loaded = false;
                chunks.push_back(chunk);
            }
            chunkNotGenerated = true;
        }
    }
}

Window::~Window() {}