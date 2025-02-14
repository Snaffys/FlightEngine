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

    //maxViewDst = terrain.detailLevels.back().visibleDstThreshold;

    camera = Camera(glm::vec3(0.0f, 300.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f), 100.0f);

    //camera = Camera(glm::vec3(67.0f, 627.5f, 169.9f), glm::vec3(0.0f, 0.0f, -1.0f),
    //glm::vec3(0.0f, 1.0f, 0.0f), 1000.0f);

    planePosition = camera.getCameraPosition() + glm::vec3(0.0f, -10.0f, -30.0f);

    terrain = Terrain(&camera);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // SHADERS
    terrainShader = Shader("shaders/vertex/test.vert", "shaders/fragment/text.frag");
    terrainShaderNormals = Shader("shaders/fragment/test3.vert", "shaders/fragment/test1.frag", "shaders/fragment/test2.geom");
    postProcShader = Shader("shaders/vertex/postProc.vert", "shaders/fragment/postProc.frag");
    cubemapShader = Shader("shaders/vertex/cubemap.vert", "shaders/fragment/cubemap.frag");
    planeShader = Shader("shaders/vertex/plane.vert", "shaders/fragment/plane.frag");
    modelShader = Shader("shaders/vertex/model.vert", "shaders/fragment/model.frag");

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

    //modelM = ("C:/ProGGraming/FlightSimulator/FlightSimulator/res/models/sword/scene.gltf");

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
}

void Window::launch() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    
    terrain.updateVisibleChunks();

    try {
        while (!glfwWindowShouldClose(window)) {
            // for proper movement for every PC
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            if (!freeCamera) {

                ProcessMouseMove1(turnRate, planeRotationDegreesVertical);
                camera.setCameraFront(planeFront);
                camera.ProcessKeyPress(FORWARD, deltaTime, planeFront, glm::vec3(0.0,1.0,0.0));
                //if(planeRotationDegreesVertical > 90 && planeRotationDegreesVertical < 270)
                //    camera.ProcessKeyPress(BACKWARD, deltaTime);
                //else
                //    camera.ProcessKeyPress(FORWARD, deltaTime);
                //camera.ProcessKeyPress(UP, planeRotationDegreesVertical / 10000.0f);
                //camera.ProcessKeyPress(RIGHT, planeRotationDegreesHorizontal / 10000.0f);
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
            if (lengthOfADistance * lengthOfADistance > sqrViewerMoveThresholdForChunkUpdate) {
                cameraPostionOld = glm::vec2(camera.getCameraPosition().x, camera.getCameraPosition().z);
                terrain.updateVisibleChunks();
            }

            terrain.updateThread();

            // Visible chunks rendering
            for (Chunk* chunk : terrain.getVisibleChunks()) {
                glm::vec2 chunkPos = chunk->getPosition();

                glm::mat4 model = glm::mat4(1.0f);

                //model = glm::translate(model, glm::vec3(chunkPos.x * terrain.getVertexWidthAmount() - 10, 0.0f, chunkPos.y * terrain.getVertexHeightAmount()));
                //printf("%d\n", chunkPos.x * terrain.getVertexWidthAmount());
                model = glm::translate(model, glm::vec3(int(chunkPos.x * (terrain.getVertexWidthAmount() - 1)), 0.0f, chunkPos.y * (terrain.getVertexHeightAmount() - 1)));

                //float posX = chunkPos.x * (terrain.getVertexWidthAmount() - 1);
               // posX = round(posX / 20.0f) * 20.0f;  // Round to the nearest multiple of 20
                //model = glm::translate(model, glm::vec3(posX, 0.0f, 0.0));

                //model = glm::translate(glm::mat4(1.0f), glm::vec3(chunkPos.x * terrain.getVertexWidthAmount() + (chunkPos.x != 0), 0.0f, chunkPos.y * terrain.getVertexHeightAmount() + (chunkPos.y != 0)));

                //model = glm::scale(model, glm::vec3(100.0f, 1.0f, 10.0f));

                terrainShader.setMat4("model", model);
                terrainShader.setFloat("posX", chunkPos.x);
                terrainShader.setFloat("posZ", chunkPos.y);

                if (chunk->getVAO().IsValid()) {
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

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glDisable(GL_CULL_FACE);

            //modelM.Draw(modelShader, camera);

            // PLANE__________________________________________________________________________________________________________
             planeShader.Activate();
             //glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / height, 0.1f, 10000000.0f);
             glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
             //glm::mat4 view = camera.GetViewMatrix();
             if (freeCamera) {
                 model = glm::translate(model, planePosition);
                 planeShader.setMat4("projection", projection);
                 planeShader.setMat4("view", view);
             }
             else {
                 model = glm::translate(model, glm::vec3(0.0f, -10.0f, -30.0f));
                 planeShader.setMat4("projection", projection);
                 planeShader.setMat4("view", glm::mat4(1.0f));
             }



             model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
             //model = glm::rotate(model, glm::radians(planeRotationDegreesVertical), glm::vec3(1, 0, 0));
             model = glm::rotate(model, glm::radians(planeRotationDegreesHorizontal), glm::vec3(0, 0, 1));
             planeShader.setMat4("model", model);

             planeVao.Bind();
             glDrawArrays(GL_TRIANGLES, 0, planeVbo.getVertexCount());
            //__________________________________________________________________________________________________________________


            glEnable(GL_CULL_FACE);


            // CUBEMAP
            glDepthFunc(GL_LEQUAL); // depth test passes when values are equal or less than depth buffer's content
            cubemapShader.Activate();
            cubemapShader.setInt("cubemap", cubemapTex.GetId());
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

            //printf("%f\n", planeRotationDegreesHorizontal);
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
                    camera.resetCamera();
                    glfwSetCursorPos(window, savedCursorPos.x, savedCursorPos.y);
                    planePosition = camera.getCameraPosition() + glm::vec3(0.0f, -10.0f, -30.0f);
                    //planeFront = camera.getCameraFront();
                    camera.setCameraPosition(camera.getCameraPosition() + glm::vec3(0.0, 15.0, 15.0));
                }
                else {
                    //camera.setCameraFront(planeFront);
                    camera.setCameraFront(glm::vec3(0.0,0.0,-1.0f));
                    //camera.setCameraFront(glm::vec3(0,0,-1));
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
            //if (planeRotationDegreesVertical < 45.0f)
                planeRotationDegreesVertical += 0.1;
            if (planeRotationDegreesVertical == 360)
                planeRotationDegreesVertical = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            //if (planeRotationDegreesVertical > -45.0f)
                planeRotationDegreesVertical -= 0.1;
            if (planeRotationDegreesVertical == -360)
                planeRotationDegreesVertical = 0;
        }
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && planeRotationDegreesVertical != 0){
            if (planeRotationDegreesVertical > 0) {
                planeRotationDegreesVertical -= 0.1;
            }
            else if (planeRotationDegreesVertical < 0) {
                planeRotationDegreesVertical += 0.1;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            if (planeRotationDegreesHorizontal < 90.0f) {
                planeRotationDegreesHorizontal += 0.1;
                if (planeRotationDegreesHorizontal >= 0.0f) {

                    turnRate += 0.1;
                }
                else {
                    turnRate -= 0.1;
                }
            }
            else {
                turnRate += 9 / planeRotationDegreesHorizontal;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if (planeRotationDegreesHorizontal > -90.0f) {
                planeRotationDegreesHorizontal -= 0.1;
                if (planeRotationDegreesHorizontal <= 0.0f) {
                    turnRate -= 0.1;
                }
                else {
                    turnRate += 0.1;
                }

                //turnRate = 900 / planeRotationDegreesHorizontal;
            }
            else {
                turnRate += 9 / planeRotationDegreesHorizontal;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE && planeRotationDegreesHorizontal != 0) {
            if (planeRotationDegreesHorizontal > 0) {
                planeRotationDegreesHorizontal -= 0.1;
                turnRate += 0.1;
            }
            else if (planeRotationDegreesHorizontal < 0) {
                planeRotationDegreesHorizontal += 0.1;
                turnRate -= 0.1;
            }
        }
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
        if (nOctaveCount == 10 || terrain.getVertexWidthAmount() < pow(2.0f, nOctaveCount - 1))
            nOctaveCount = 1;

        terrain.updateVisibleChunks();


        printf("\noctave count:%u", nOctaveCount);

    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        //for (unsigned int i = 0; i < vertexHeightAmount * vertexWidthAmount; i++)
        //    noiseSeed2D[i] = (float)rand() / (float)RAND_MAX;	// random values [0.0, 1.0]

        terrain.updateVisibleChunks();
    }


    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        fScalingBias += 0.2;

        terrain.updateVisibleChunks();


        printf("\nscaling bias:%f", fScalingBias);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        fScalingBias -= 0.2;
        if (fScalingBias < 0.2)
            fScalingBias = 0.2;

        terrain.updateVisibleChunks();

        printf("\nscaling bias:%f", fScalingBias);
    }

}

void Window::ProcessMouseMove1(float x_offset, float y_offset) {


    // Calculates directions
    glm::vec3 direction;
    direction.z = -(cos(glm::radians(x_offset)) * cos(glm::radians(y_offset)));
    direction.y = sin(glm::radians(y_offset));
    direction.x = -(sin(glm::radians(x_offset)) * cos(glm::radians(y_offset)));
    // Sets camera_front
    planeFront = glm::normalize(direction);
}


Window::~Window() {}