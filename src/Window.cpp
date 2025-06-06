#include <Window.h>
#include <utility>

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

    //maxViewDst = terrain.detailLevels.back().visibleDstThreshold;

    camera = Camera(glm::vec3(0.0f, 800.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f), 100.0f);

    //camera = Camera(glm::vec3(67.0f, 627.5f, 169.9f), glm::vec3(0.0f, 0.0f, -1.0f),
    //glm::vec3(0.0f, 1.0f, 0.0f), 1000.0f);

    planePosition = camera.getCameraPosition() + glm::vec3(0.0f, -10.0f, -30.0f);

    //terrain = Terrain(&camera);
    //terrain = std::move(Terrain(&camera));
    terrain = std::make_unique<Terrain>(&camera, &db);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);



    // SHADERS
    terrainShader = Shader("shaders/vertex/test.vert", "shaders/fragment/text.frag");
    terrainShaderNormals = Shader("shaders/fragment/test3.vert", "shaders/fragment/test1.frag", "shaders/fragment/test2.geom");
    postProcShader = Shader("shaders/vertex/postProc.vert", "shaders/fragment/postProc.frag");
    cubemapShader = Shader("shaders/vertex/cubemap.vert", "shaders/fragment/cubemap.frag");
    planeShader = Shader("shaders/vertex/plane.vert", "shaders/fragment/plane.frag");
    modelShader = Shader("shaders/vertex/model.vert", "shaders/fragment/plane.frag");

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
    postProcVbo = {
        // vertex		    // texCoords
        {-1.0f,  1.0f,		0.0f, 1.0f},
        { 1.0f, -1.0f,		1.0f, 0.0f},
        {-1.0f, -1.0f,		0.0f, 0.0f},
        {-1.0f,  1.0f,		0.0f, 1.0f},
        { 1.0f,  1.0f,		1.0f, 1.0f},
        { 1.0f, -1.0f,		1.0f, 0.0f}
    };
    postProcVao = PosTex2DVao(postProcVbo);


    planeVbo = {
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


    //planeModel = Model("res/models/plane/scene.gltf");
    planeModel = Model("res/models/mig35/mig35.glb");




    ParticleGenerator::Config snowConfig;
    snowConfig.baseVelocity = glm::vec3(0, -15.0f, 0); // Fast downward fall
    snowConfig.velocityRandomness = glm::vec3(2.0f, 0.5f, 2.0f); // Some horizontal variation
    snowConfig.startColor = glm::vec4(0.7f, 0.8f, 1.0f, 1.0f); // Light blue
    snowConfig.endColor = glm::vec4(0.7f, 0.8f, 1.0f, 0.0f); // Fade to transparent
    snowConfig.sizeStart = 0.05f;
    snowConfig.sizeEnd = 0.1f;
    snowConfig.minLifetime = 1.0f;
    snowConfig.maxLifetime = 1.5f;

    ParticleGenerator::Config fireConfig;
    fireConfig.baseVelocity = glm::vec3(0, 5.0f, 0); // Upward movement
    fireConfig.velocityRandomness = glm::vec3(1.5f, 2.0f, 1.5f); // Swirling motion
    fireConfig.startColor = glm::vec4(1.0f, 0.3f, 0.1f, 1.0f); // Bright orange
    fireConfig.endColor = glm::vec4(1.0f, 0.8f, 0.2f, 0.0f); // Fade to yellow then transparent
    fireConfig.sizeStart = 0.3f;
    fireConfig.sizeEnd = 0.1f; // Shrink as they rise
    fireConfig.minLifetime = 0.5f;
    fireConfig.maxLifetime = 1.5f;

    // Custom behavior for rain splashes
    snowConfig.behaviorUpdate = [](Particle& p, float dt) {
        // Add slight wind effect
        p.velocity.x += sin(glfwGetTime() * 2.0f) * 0.1f * dt;

        // Create splash when hitting "ground" (y < 0)
        if (p.position.y < 0.0f && p.life > 0.1f) {
            p.life = 0.1f; // Quickly fade out
            p.velocity = glm::vec3(0); // Stop movement
        }
        };

    snowParticles = ParticleGenerator(1000, snowConfig);


    // Custom fire behavior
    fireConfig.behaviorUpdate = [](Particle& p, float dt) {
        // Swirling motion
        p.velocity.x += sin(glfwGetTime() * 5.0f + p.position.y) * 0.5f * dt;
        p.velocity.z += cos(glfwGetTime() * 5.0f + p.position.y) * 0.5f * dt;

        // Gravity effect (weaker than normal)
        p.velocity.y -= 1.0f * dt;

        // Random flickering
        p.colorStart.r += (rand() % 100 / 100.0f - 0.5f) * 0.1f;
        p.colorStart.g += (rand() % 100 / 100.0f - 0.3f) * 0.1f;
        };

    fireParticles = ParticleGenerator(500, fireConfig);
}

void Window::launch() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);

    terrain->updateVisibleChunks();

    try {
        lastFrame = glfwGetTime();
        while (!glfwWindowShouldClose(window)) {


            // for proper movement for every PC
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);



            if (!freeCamera) {
                yawChange();


                planePosition = aircraft.update(
                    camera.getCameraPosition(),
                    planeFront,
                    deltaTime,
                    planeUp,
                    planeRight
                );
                // Update camera position
                camera.setCameraPosition(planePosition - glm::vec3(0.0f, -10.0f, -30.0f));
            }

            postProcFbo.BindFramebuffer();
            glEnable(GL_DEPTH_TEST);

            glClearColor(0.1f, 0.3f, 0.6f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            terrainShader.Activate();

            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / height, 0.1f, 10000000.0f);
            terrainShader.setMat4("projection", projection);
            glm::mat4 view = camera.getViewMatrix();
            terrainShader.setMat4("view", view);

            //glm::mat4 model = glm::mat4(1.0f);
            //model = glm::scale(model, glm::vec3(1000.0f, 500.0f, 1000.0f));
           // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            //terrainShader.setMat4("model", model);

            if (polygonMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glDisable(GL_CULL_FACE);


            float lengthOfADistance = glm::length(cameraPostionOld - glm::vec2(camera.getCameraPosition().x, camera.getCameraPosition().z));
            

            //std::cout << cameraPostionOld.x << "\t" << cameraPostionOld.y << "\t" << camera.getCameraPosition().x << "\t" << camera.getCameraPosition().z << std::endl;
            
            //std::cout << lengthOfADistance << std::endl;


            if (lengthOfADistance * lengthOfADistance > sqrViewerMoveThresholdForChunkUpdate) {
                cameraPostionOld = glm::vec2(camera.getCameraPosition().x, camera.getCameraPosition().z);
                terrain->updateVisibleChunks();
                //db.debugPrintAllChunks();
            }
            
            terrain->updateThread();



            //std::cout << terrain->chunks.size() << std::endl;
            //std::cout << camera.getCameraPosition().x << std::endl;

            // Visible chunks rendering
            for (Chunk* chunk : terrain->getVisibleChunks()) {
                //if (chunk->vao && chunk->getNumStrips() > 0 && chunk->getNumVertsPerStrip() > 0){
                glm::vec2 chunkPos = chunk->getPosition();

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(int(chunkPos.x * (terrain->getVertexWidthAmount() - 1)), -300.0f, chunkPos.y * (terrain->getVertexHeightAmount() - 1)));

                terrainShader.setMat4("model", model);
                terrainShader.setFloat("posX", chunkPos.x);
                terrainShader.setFloat("posZ", chunkPos.y);


                if (chunk->vao) {
                    chunk->getVAO().Bind();

                    terrainShader.Activate();
                    terrainShader.setMat4("projection", projection);
                    terrainShader.setMat4("view", view);
                    terrainShader.setMat4("model", model);
                    terrainShader.setFloat("posX", chunkPos.x);
                    terrainShader.setFloat("posZ", chunkPos.y);

                    // render the mesh triangle strip by triangle strip - each row at a time
                    for (unsigned int strip = 0; strip < chunk->getNumStrips(); ++strip)
                    {
                        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                            chunk->getNumVertsPerStrip(), // number of indices to render
                            GL_UNSIGNED_INT,     // index data type
                            (void*)(sizeof(unsigned int)
                                * chunk->getNumVertsPerStrip()
                                * strip)); // offset to starting index
                    }


                    //terrainShaderNormals.Activate();
                    //terrainShaderNormals.setMat4("projection", projection);
                    //terrainShaderNormals.setMat4("view", view);
                    //terrainShaderNormals.setMat4("model", model);
                    //terrainShaderNormals.setFloat("posX", chunkPos.x);
                    //terrainShaderNormals.setFloat("posZ", chunkPos.y);

                    //chunk->getVAO().Bind();

                    //// render the mesh triangle strip by triangle strip - each row at a time
                    //for (unsigned int strip = 0; strip < chunk->getNumStrips(); ++strip)
                    //{
                    //    glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                    //        chunk->getNumVertsPerStrip(), // number of indices to render
                    //        GL_UNSIGNED_INT,     // index data type
                    //        (void*)(sizeof(unsigned int)
                    //            * chunk->getNumVertsPerStrip()
                    //            * strip)); // offset to starting index
                    //}
                }
            }


            //// Visible chunks rendering
            //for (const auto& pair : terrain->chunks) {
            //    const glm::vec2& chunkPos = pair.first;
            //    Chunk chunk = pair.second;

            //    glm::mat4 model = glm::mat4(1.0f);
            //    model = glm::translate(model, glm::vec3(int(chunkPos.x * (terrain->getVertexWidthAmount() - 1)), -300.0f, chunkPos.y * (terrain->getVertexHeightAmount() - 1)));

            //    terrainShader.setMat4("model", model);
            //    terrainShader.setFloat("posX", chunkPos.x);
            //    terrainShader.setFloat("posZ", chunkPos.y);


            //    //if (chunk->vao) {
            //    if (chunk.getVAO().IsValid()) {
            //        chunk.getVAO().Bind();

            //        terrainShader.Activate();
            //        terrainShader.setMat4("projection", projection);
            //        terrainShader.setMat4("view", view);
            //        terrainShader.setMat4("model", model);
            //        terrainShader.setFloat("posX", chunkPos.x);
            //        terrainShader.setFloat("posZ", chunkPos.y);

            //        // render the mesh triangle strip by triangle strip - each row at a time
            //        for (unsigned int strip = 0; strip < chunk.getNumStrips(); ++strip)
            //        {
            //            glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
            //                chunk.getNumVertsPerStrip(), // number of indices to render
            //                GL_UNSIGNED_INT,     // index data type
            //                (void*)(sizeof(unsigned int)
            //                    * chunk.getNumVertsPerStrip()
            //                    * strip)); // offset to starting index
            //        }
            //    }
            //}

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glDisable(GL_CULL_FACE);





            // MODEL______________________________________________________________________________________________________

            //glm::quat yawRotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat yawRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 planeScaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(0.4));

            //glm::quat yawRotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            //glm::quat rollRotation = glm::angleAxis(glm::radians(-planeRotationDegreesYaw), glm::vec3(0.0f, 0.0f, 1.0f));

            //planeModel.renderModel(modelShader, view, projection, freeCamera, planePosition, yawRotation * rollRotation);
            if(freeCamera)
                planeModel.renderModel(modelShader, view, projection, freeCamera, planePosition, planeOrientation * yawRotation, planeScaleMatrix);
            else
                planeModel.renderModel(modelShader, view, projection, freeCamera, planePosition, yawRotation, planeScaleMatrix);

            //____________________________________________________________________________________________________________

            glEnable(GL_CULL_FACE);

            // Update rain
            snowParticles.Update(deltaTime, 20,
                glm::vec3(200.0f, 0, 200.0f), // Area size (wide and flat)
                planePosition + glm::vec3(0, 10.0f, 0)); // Start above emitter

            // Update fire
            fireParticles.Update(deltaTime, 5,
                glm::vec3(2.0f, 0.5f, 2.0f), // Small area
                planePosition); // Start at emitter



            snowParticles.Draw(view, projection, camera.getCameraUp(), glm::normalize(glm::cross(camera.getCameraFront(), camera.getCameraUp())));

            //fire.Update(deltaTime, 10, glm::vec3(200.0f, 50.0f, 200.0f), planePosition);
            fireParticles.Draw(view, projection, camera.getCameraUp(), glm::normalize(glm::cross(camera.getCameraFront(), camera.getCameraUp())));


            // CUBEMAP
            glDepthFunc(GL_LEQUAL); // depth test passes when values are equal or less than depth buffer's content
            cubemapShader.Activate();
            cubemapShader.setInt("cubemap", cubemapTex.GetId());
            //cubemapShader.setFloat("time", currentFrame);
            //float blendFactor = (sin(currentFrame) + 1.0f) / 2.0f;
            //cubemapShader.setFloat("blendFactor", blendFactor);


            glm::mat4 cubemapView = glm::mat4(glm::mat3(camera.getViewMatrix()));
            cubemapShader.setMat4("projection", projection);
            cubemapShader.setMat4("view", cubemapView);
            cubemapVao.Bind();
            glDrawArrays(GL_TRIANGLES, 0, cubemapVbo.getVertexCount());
            glDepthFunc(GL_LESS);   // set depth function to default

            postProcFbo.UnbindFramebuffer();
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);





            // FRAMEBUFFER
            postProcShader.Activate();
            postProcVao.Bind();
            postProcShader.setInt("screenTexture", postProcTex.GetId());
            postProcShader.setFloat("exposure", exposure);
            glDrawArrays(GL_TRIANGLES, 0, postProcVbo.getVertexCount());

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
    if (windowInstance->freeCamera) {
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
}

void Window::processInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (!shiftWasPressed) {
            shiftWasPressed = true;
            freeCamera = !freeCamera;
            if (freeCamera) {
                firstMouse = true;

                glm::vec3 front = planeFront;
                float newPitch = glm::degrees(asin(front.y));
                float newYaw = glm::degrees(atan2(front.z, front.x));
                camera.setYaw(newYaw);
                camera.setPitch(newPitch);
                camera.resetRoll();
                camera.setCameraFront(front);

                glfwSetCursorPos(window, 0, 0);

                planePosition = camera.getCameraPosition();
                glm::vec3 worldUp = glm::vec3(0.0f, 20.0f, 0.0f);

                windowInstance->camera.setCameraUp(glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

                camera.setCameraPosition(camera.getCameraPosition() + worldUp + (-45.0f * planeFront));

            }
            else {
                camera.setCameraFront(planeFront);
                camera.setCameraRight(glm::normalize(glm::cross(planeFront, planeUp)));
                camera.setCameraUp(glm::normalize(planeUp));
                camera.setCameraPosition(planePosition - glm::vec3(0.0f, -10.0f, -30.0f));
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        shiftWasPressed = false;
    }


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (freeCamera) {
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
        //if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        //    camera.ProcessKeyPress(UP, deltaTime);
        //}
        //if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        //    camera.ProcessKeyPress(DOWN, deltaTime);
        //}
    }

    if (!freeCamera) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            pitchChange(0.1f);
            resetRollAngle = false;
            resetPitchAngle = false;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            pitchChange(-0.1f);
            resetRollAngle = false;
            resetPitchAngle = false;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            rollChange(0.1f);
            resetRollAngle = false;
            resetPitchAngle = false;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            rollChange(-0.1f);
            resetRollAngle = false;
            resetPitchAngle = false;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            resetRollAngle = true;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            resetPitchAngle = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        polygonMode = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        polygonMode = true;


    //if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
    //    while (planeRotationDegreesRoll != 0) {
    //        planeRotationDegreesRoll += 0.2 * (planeRotationDegreesRoll / abs(planeRotationDegreesRoll));
    //        rollChange(0.2 * (planeRotationDegreesRoll / abs(planeRotationDegreesRoll)));
    //    }
    //}
    //
    //if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
    //    polygonMode = true;
    //}


    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
    {
        octavePressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !octavePressed) {
        nOctaveCount++;
        octavePressed = true;
        if (nOctaveCount == 10 || terrain->getVertexWidthAmount() < pow(2.0f, nOctaveCount - 1))
            nOctaveCount = 1;

        terrain->updateVisibleChunks();


        printf("\noctave count:%u", nOctaveCount);

    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        //for (unsigned int i = 0; i < vertexHeightAmount * vertexWidthAmount; i++)
        //    noiseSeed2D[i] = (float)rand() / (float)RAND_MAX;	// random values [0.0, 1.0]

        terrain->updateVisibleChunks();
    }


    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        fScalingBias += 0.2;

        terrain->updateVisibleChunks();


        printf("\nscaling bias:%f", fScalingBias);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        fScalingBias -= 0.2;
        if (fScalingBias < 0.2)
            fScalingBias = 0.2;

        terrain->updateVisibleChunks();

        printf("\nscaling bias:%f", fScalingBias);
    }

}

void Window::resetRoll() {
    //// Keep current pitch direction (planeFront.y unchanged)
    //glm::vec3 front = planeFront;

    //// Compute a new up vector that is perpendicular to front (removes bank)
    //glm::vec3 newUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) - front * glm::dot(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    //// Rebuild orientation
    //glm::mat4 newOrientation = glm::lookAt(glm::vec3(0.0f), front, newUp);
    //planeOrientation = glm::quat_cast(newOrientation);

    //// Update vectors
    //planeFront = front;
    //planeUp = newUp;
    //planeRight = glm::normalize(glm::cross(planeFront, planeUp));

    //// Update camera
    //camera.setCameraFront(planeFront);
    //camera.setCameraRight(planeRight);
    //camera.setCameraUp(planeUp);
}

void Window::yawChange() {
    float yawRate = 0.0f;
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    // (1 = upright, -1 = inverted)
    float rollInfluence = glm::dot(planeRight, worldUp);
    yawRate = rollInfluence * 0.05f;

    float pitchEffect = 1.0f - fabs(planeFront.y);
    yawRate *= pitchEffect;


    glm::quat yawQuat;

    if (planeUp.y < 0)
        yawQuat = glm::angleAxis(glm::radians(yawRate), glm::vec3(0.0f, -1.0f, 0.0f));
    else
        yawQuat = glm::angleAxis(glm::radians(yawRate), glm::vec3(0.0f, 1.0f, 0.0f));
    planeOrientation = glm::normalize(yawQuat * planeOrientation);

    planeFront = glm::normalize(glm::rotate(planeOrientation, glm::vec3(0.0f, 0.0f, -1.0f)));
    planeUp = glm::normalize(glm::rotate(planeOrientation, glm::vec3(0.0f, 1.0f, 0.0f)));
    planeRight = glm::normalize(glm::cross(planeFront, planeUp));

    camera.setCameraFront(planeFront);
    camera.setCameraRight(planeRight);
    camera.setCameraUp(planeUp);
}

void Window::rollChange(float roll) {
    glm::quat rollQuat = glm::angleAxis(glm::radians(roll), planeFront);
    planeOrientation = glm::normalize(rollQuat * planeOrientation);

    planeUp = glm::rotate(planeOrientation, glm::vec3(0.0f, 1.0f, 0.0f));
    planeRight = glm::normalize(glm::cross(planeFront, planeUp));
    
    camera.setCameraRight(planeRight);
    camera.setCameraUp(glm::normalize(planeUp));
}

void Window::pitchChange(float pitch) {
    //glm::vec3 right = glm::normalize(glm::cross(planeFront, planeUp));
    glm::quat pitchQuat = glm::angleAxis(glm::radians(pitch), planeRight); 
    planeOrientation = glm::normalize(pitchQuat * planeOrientation);

    planeFront = glm::normalize(glm::rotate(planeOrientation, glm::vec3(0.0f, 0.0f, -1.0f)));
    planeUp = glm::normalize(glm::rotate(planeOrientation, glm::vec3(0.0f, 1.0f, 0.0f)));
    planeRight = glm::normalize(glm::cross(planeFront, planeUp));

    camera.setCameraFront(planeFront);
    camera.setCameraRight(planeRight);
    camera.setCameraUp(glm::normalize(planeUp));
}



void Window::processCameraRotation(float turnRate, float pitch, float roll) {

    glm::quat yawQuat = glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 1, 0));

    // Start with identity quaternion (or previous rotation state)
    //glm::quat finalQuat = glm::quat(1, 0, 0, 0);

    // 1. Use the current forward vector (`cameraFront` or `planeFront`)
    glm::vec3 forward = glm::normalize(camera.getCameraFront());  // Use the actual plane direction

    //// 2. Compute right and up vectors based on the current orientation
    //glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0))); // World Up
    //glm::vec3 up = glm::normalize(glm::cross(right, forward));

    // 3. Apply roll **around current forward direction**
    glm::quat rollQuat = glm::angleAxis(glm::radians(roll), forward);
    //finalQuat = rollQuat * finalQuat;

    // 4. Update right vector after roll
    glm::vec3 right = glm::rotate(rollQuat, glm::vec3(1.0f, 0.0f, 0.0f));

    // 5. Apply pitch **around the new right vector**
    glm::quat pitchQuat = glm::angleAxis(glm::radians(pitch), right);
    glm::quat finalQuat = rollQuat * pitchQuat * yawQuat;

    //glm::quat finalQuat = rollQuat;
    //glm::quat finalQuat = rollQuat * pitchQuat;

    // 6. Compute final directions after roll & pitch
    planeFront = glm::normalize(glm::rotate(finalQuat, glm::vec3(0.0f, 0.0f, -1.0f)));
    planeUp = glm::normalize(glm::rotate(finalQuat, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 cameraRight = glm::normalize(glm::cross(planeFront, planeUp));

    // Debug output

    // Apply to camera
    //camera.setCameraFront(planeFront);
    camera.setCameraRight(cameraRight);
    camera.setCameraUp(glm::normalize(planeUp));
}

Window::~Window() {}