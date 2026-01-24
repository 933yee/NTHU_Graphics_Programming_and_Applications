#include "../Include/Common.h"
using namespace glm;
using namespace std;

GLuint          program;
GLuint          texure_object[10];
GLuint          tex_index;
GLuint          vao[10];

struct
{
    GLint       mv_matrix;
    GLint       proj_matrix;
} uniforms;

float angleX = 0.0f; 
float angleY = 0.0f; 
int lastMouseX, lastMouseY; 
bool isMouseDown = false;
float zoom = 1.0f;
float robot_offsetx = 0.0f;
float robot_offsety = 0.0f;

bool enable_animation = true;

static const char* render_fs_glsl[] =
{
    "#version 410 core                                            \n"
    "                                                             \n"
    "uniform sampler2D tex_object;                                \n"
    "                                                             \n"
    "in VS_OUT                                                    \n"
    "{                                                            \n"
    "    vec2 tc;                                                 \n"
    "} fs_in;                                                     \n"
    "                                                             \n"
    "out vec4 color;                                              \n"
    "                                                             \n"
    "void main(void)                                              \n"
    "{                                                            \n"
    "    color = vec4(1.0);                                                     \n"
    "    color = texture(tex_object, fs_in.tc * vec2(3.0, 1.0));  \n"
    "}                                                            \n"
};

static const char* render_vs_glsl[] =
{
    "#version 410 core                            \n"
    "                                             \n"
    "uniform mat4 mv_matrix;                      \n"
    "uniform mat4 proj_matrix;                    \n"
    "                                             \n"
    "layout (location = 0) in vec3 position;      \n"
    "layout (location = 1) in vec2 tc;            \n"
    "                                             \n"
    "out VS_OUT                                   \n"
    "{                                            \n"
    "    vec2 tc;                                 \n"
    "} vs_out;                                    \n"
    "                                             \n"
    "void main(void)                              \n"
    "{                                            \n"
    "    vec4 pos_vs = mv_matrix * vec4(position, 1.0);      \n"
    "                                             \n"
    "    vs_out.tc = tc;                          \n"
    "                                             \n"
    "    gl_Position = proj_matrix * pos_vs;      \n"
    "}                                            \n"
};


char* texture_paths[10] = {
    "../Media/Textures/pink.jpeg",
    "../Media/Textures/yellow.jpeg",
    "../Media/Textures/blue.jpeg",
    "../Media/Textures/orange.jpeg",
    "../Media/Textures/green.jpeg",
    "../Media/Textures/red.jpeg",
    "../Media/Textures/skyblue.jpeg",
    "../Media/Textures/skyblue.jpeg",
    "../Media/Textures/skyblue.jpeg",
    "../Media/Textures/skyblue.jpeg",
};

char* model_paths[10] = {
    "../Media/Objects/T.obj",
    "../Media/Objects/O.obj",
    "../Media/Objects/L.obj",
    "../Media/Objects/J.obj",
    "../Media/Objects/S.obj",
    "../Media/Objects/Z.obj",
    "../Media/Objects/I.obj",
    "../Media/Objects/I.obj",
    "../Media/Objects/I.obj",
    "../Media/Objects/I.obj",
};

vector<MeshData> models;
vector<TextureData> mesh_textures;

void generate_textures() {
    for (int i = 0; i < mesh_textures.size(); i++) {
        glGenTextures(1, &texure_object[i]);
        glBindTexture(GL_TEXTURE_2D, texure_object[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mesh_textures[i].width, mesh_textures[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mesh_textures[i].data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}

void load_textures() {
    for (auto& path : texture_paths) {
        TextureData texture = loadImg(path);
        mesh_textures.push_back(texture);
    }
}

void load_models() {
    for (auto& path : model_paths) {
        vector<MeshData> mesh = loadObj(path);
        models.push_back(mesh[0]);
    }
}


void My_Init()
{
    program = glCreateProgram();
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, render_fs_glsl, NULL);
    glCompileShader(fs);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, render_vs_glsl, NULL);
    glCompileShader(vs);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    //printGLShaderLog(vs);
    //printGLShaderLog(fs);

    glLinkProgram(program);
    glUseProgram(program);

    load_models();
    load_textures();
    generate_textures();

    uniforms.mv_matrix = glGetUniformLocation(program, "mv_matrix");
    uniforms.proj_matrix = glGetUniformLocation(program, "proj_matrix");
    for (int i = 0; i < 10; i++) {
        
        glGenVertexArrays(1, &vao[i]);
        glBindVertexArray(vao[i]);
        GLuint position_buffer;
        GLuint texcoord_buffer;
        GLuint index_buffer;

        glGenBuffers(1, &position_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
        glBufferData(GL_ARRAY_BUFFER, models[i].positions.size() * sizeof(float), models[i].positions.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &texcoord_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
        glBufferData(GL_ARRAY_BUFFER, models[i].texcoords.size() * sizeof(float), models[i].texcoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, models[i].indices.size() * sizeof(unsigned int), models[i].indices.data(), GL_STATIC_DRAW);
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}


mat4 torso_matrix = rotate(mat4(1.0f), deg2rad(90.0f), vec3(1.0f, 0.0f, 0.0f)) * rotate(mat4(1.0f), deg2rad(180.0f), vec3(0.0f, 1.0f, 0.0f));

// Head
mat4 head_matrix = rotate(mat4(1.0f), deg2rad(20.0f), vec3(1.0f, 0.0f, 0.0f)) * torso_matrix * scale(mat4(1.0f), vec3(0.8f));

// Left Arm
mat4 left_arm_matrix = rotate(mat4(1.0f), deg2rad(-10), vec3(0.0f, 0.0f, 1.0f)) * rotate(mat4(1.0f), deg2rad(10), vec3(1.0f, 0.0f, 0.0f)) * rotate(mat4(1.0f), deg2rad(90), vec3(0.0f, 1.0f, 0.0f)) * torso_matrix * scale(mat4(1.0f), vec3(1.2f));

// Right Arm
mat4 right_arm_matrix = rotate(mat4(1.0f), deg2rad(10), vec3(0.0f, 0.0f, 1.0f)) * rotate(mat4(1.0f), deg2rad(10), vec3(1.0f, 0.0f, 0.0f)) * rotate(mat4(1.0f), deg2rad(90), vec3(0.0f, 1.0f, 0.0f)) * torso_matrix * scale(mat4(1.0f), vec3(1.2f));

// Left Leg
mat4 left_leg_matrix = rotate(mat4(1.0f), deg2rad(100), vec3(1.0f, 0.0f, 0.0f)) * rotate(mat4(1.0f), deg2rad(90), vec3(0.0f, 1.0f, 0.0f)) * torso_matrix * scale(mat4(1.0f), vec3(0.8f));

// Right Leg
mat4 right_leg_matrix = rotate(mat4(1.0f), deg2rad(80), vec3(1.0f, 0.0f, 0.0f)) * rotate(mat4(1.0f), deg2rad(90), vec3(0.0f, 1.0f, 0.0f)) * torso_matrix * scale(mat4(1.0f), vec3(0.8f));

// Left Arm 2
mat4 left_arm2_matrix = left_arm_matrix * scale(mat4(1.0f), vec3(0.6f));

// Right Arm 2
mat4 right_arm2_matrix = right_arm_matrix * scale(mat4(1.0f), vec3(0.6f));

// Left Leg 2
mat4 left_leg2_matrix = left_leg_matrix;

// Right Leg 2
mat4 right_leg2_matrix = right_leg_matrix;

void My_Display()
{
    static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    static const GLfloat ones[] = { 1.0f };

    glClearBufferfv(GL_COLOR, 0, gray);
    glClearBufferfv(GL_DEPTH, 0, ones);

    float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

    glUseProgram(program);



    //mat4 init_pos[10] = {
    //    //torso
    //    translate(mat4(1.0f), vec3(0.0f, 2.0f, -20.0f)) * rotate(mat4(1.0f), deg2rad(180.0f), vec3(0.0f, 1.0f, 0.0f)) * rotate(mat4(1.0f), deg2rad(-110.0f), vec3(1.0f, 0.0f, 0.0f)),
    //    //head
    //    translate(mat4(1.0f), vec3(0.0f, 4.5f, -20.0f)) * rotate(mat4(1.0f), deg2rad(90.0f), vec3(1.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.6f)),
    //    //left arm
    //    translate(mat4(1.0f), vec3(-3.5f, 3.5f, -20.0f))* rotate(mat4(1.0f), deg2rad(-10.0f), vec3(0.0f, 0.0f, 1.0f))*rotate(mat4(1.0f), deg2rad(-90.0f), vec3(0.0f, 1.0f, 0.0f))* rotate(mat4(1.0f), deg2rad(-90.0f), vec3(1.0f, 0.0f, 0.0f)),
    //    //right arm
    //    translate(mat4(1.0f), vec3(3.5f, 3.5f, -20.0f))* rotate(mat4(1.0f), deg2rad(10.0f), vec3(0.0f, 0.0f, 1.0f))* rotate(mat4(1.0f), deg2rad(-90.0f), vec3(0.0f, 1.0f, 0.0f))* rotate(mat4(1.0f), deg2rad(-90.0f), vec3(1.0f, 0.0f, 0.0f)),
    //    //left leg
    //    translate(mat4(1.0f), vec3(2.0f, -3.0f, -21.0f))* rotate(mat4(1.0f), deg2rad(100.0f), vec3(0.0f, 0.0f, 1.0f))* scale(mat4(1.0f), vec3(0.8f)),
    //    //right leg
    //    translate(mat4(1.0f), vec3(-2.0f, -3.0f, -21.0f))* rotate(mat4(1.0f), deg2rad(80.0f), vec3(0.0f, 0.0f, 1.0f))* scale(mat4(1.0f), vec3(0.8f)),
    //    //left arm2
    //    translate(mat4(1.0f), vec3(-4.5f, -2.f, -20.0f))* rotate(mat4(1.0f), deg2rad(-90.0f), vec3(1.0f, 0.0f, 0.0f))* rotate(mat4(1.0f), deg2rad(80.0f), vec3(0.0f, 0.0f, 1.0f))* scale(mat4(1.0f), vec3(0.7f)),
    //    //right arm2
    //    translate(mat4(1.0f), vec3(4.5f, -2.f, -20.0f))* rotate(mat4(1.0f), deg2rad(-90.0f), vec3(1.0f, 0.0f, 0.0f))* rotate(mat4(1.0f), deg2rad(100.0f), vec3(0.0f, 0.0f, 1.0f))* scale(mat4(1.0f), vec3(0.7f)),
    //    //left leg2
    //    translate(mat4(1.0f), vec3(-3.0f, -8.0f, -21.0f)) * rotate(mat4(1.0f), deg2rad(80.0f), vec3(0.0f, 0.0f, 1.0f)),
    //    //right leg2
    //    translate(mat4(1.0f), vec3(3.0f, -8.0f, -21.0f))* rotate(mat4(1.0f), deg2rad(100.0f), vec3(0.0f, 0.0f, 1.0f)),
    //};

    float speed = 15;
    float angle = 20;
    float rotation = sin(currentTime * speed) * angle;
    float torso_rotation = sin(currentTime * speed)*angle/2;
    float jump = sin(currentTime  * speed) * 0.5f;
    if (!enable_animation) {
        rotation = 0.0f;
        jump = 0.0f;
        torso_rotation = 0.0f;
    }

    mat4 transform[10] = {
        //torso
        rotate(mat4(1.0f), deg2rad(torso_rotation), vec3(1.0f, 0.0f, 0.0f))*translate(mat4(1.0f), vec3(0.0f, 2.0f, 0.0f)),
        //head
        rotate(mat4(1.0f), deg2rad(torso_rotation), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(0.0f, 4.5f, .5f)) ,
        //left arm
        rotate(mat4(1.0f), deg2rad(rotation), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(-3.5f, 3.5f, 0.5f)),
        //right arm
        rotate(mat4(1.0f), deg2rad(-rotation), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(3.5f, 3.5f, 0.5f)),
        //left leg
        rotate(mat4(1.0f), deg2rad(-rotation), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(-2.0f, -2.5f, -1.0f)),
        //right leg
        rotate(mat4(1.0f), deg2rad(rotation ), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(2.0f, -2.5f, -1.0f)),
        //left arm2
        rotate(mat4(1.0f), deg2rad(rotation), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(-4.5f, -2.f, 1.5f)),
        //right arm2
        rotate(mat4(1.0f), deg2rad(-rotation), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(4.5f, -2.f, 1.5f)),
        //left leg2
        rotate(mat4(1.0f), deg2rad(-rotation *1.5f), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(-2, -8.0f, -0.5f)),
        //right leg2
        rotate(mat4(1.0f), deg2rad(rotation*1.5f), vec3(1.0f, 0.0f, 0.0f))* translate(mat4(1.0f), vec3(2, -8.0f, -0.5f)),
    };
      
    mat4 init_pos[10] = {
        //torso
       torso_matrix,
        //head
        head_matrix,
        //left arm
       left_arm_matrix,
        //right arm
       right_arm_matrix,
        //left leg
       left_leg_matrix,
        //right leg
        right_leg_matrix,
        //left arm2
       left_arm2_matrix,
        //right arm2
        right_arm2_matrix,
        //left leg2
        left_leg2_matrix,
        //right leg2
        right_leg2_matrix,
    };
    //init_pos[1] = init_pos[0] * translate(mat4(1.0f), vec3(0.0f, 4.5f, 0.0f)) * rotate(mat4(1.0f), deg2rad(90.0f), vec3(1.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.6f));
    //init_pos[2] = translate(mat4(1.0f), vec3(0.0f, -1.0f, 0.0f))* rotate(mat4(1.0f), deg2rad(90.0f*currentTime), vec3(0.0f, 1.0f, 0.0f)) * init_pos[0];
    mat4 proj_matrix = perspective(deg2rad(80.0f), 1.0f, 0.1f, 1000.0f);

    for (int i = 0; i < 10; i++) {
        glBindVertexArray(vao[i]);

        glBindTexture(GL_TEXTURE_2D, texure_object[i]);

       // mat4 mv_matrix = init_pos[i];
            /**rotate(mat4(1.0f), deg2rad(180.0f), vec3(0.0f, 1.0f, 0.0f))
            *rotate(mat4(1.0f), deg2rad(-90.0f), vec3(1.0f, 0.0f, 0.0f));*/
            //rotate(mat4(1.0f), deg2rad((float)currentTime * 21.1f), vec3(0.0f, 0.0f, 1.0f));
        float zoomFactor = glm::mix(-10, 15, zoom);

        mat4 mv_matrix = translate(mat4(1.0f), vec3(robot_offsetx, robot_offsety + jump, -30.0f + zoomFactor))*rotate(mat4(1.0f), deg2rad(angleY), vec3(0.0f, 1.0f, 0.0f)) * rotate(mat4(1.0f), deg2rad(angleX), vec3(1.0f, 0.0f, 0.0f)) * transform[i] * init_pos[i];
        glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, &mv_matrix[0][0]);
        glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, &proj_matrix[0][0]);

        glDrawElements(GL_TRIANGLES, models[i].indices.size(), GL_UNSIGNED_INT, 0);
    }

    glutSwapBuffers();
}

void My_Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN) {
            isMouseDown = true;
            lastMouseX = x;
            lastMouseY = y;
        }
        else {
            isMouseDown = false;
        }
    }

    if (button == 3) {
        zoom += 0.1f;
    }
    else if (button == 4) {
        zoom -= 0.1f;
    }

    if (zoom < 0.1f) zoom = 0.1f;
    if (zoom > 1.0f) zoom = 1.0f;

    glutPostRedisplay();
}


void My_MouseMotion(int x, int y)
{
    if (isMouseDown)
    {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        angleY += deltaX * 0.5f; 
        angleX += deltaY * 0.5f;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay(); 
    }
}

void My_Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

void My_Timer(int val)
{
    glutPostRedisplay();
    glutTimerFunc(16, My_Timer, val);
}

void My_Menu(int val)
{
    enable_animation = val == 0;
}

void My_Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
    case 'W':
        robot_offsety += 0.5f;
        break;
    case 'a':
    case 'A':
        robot_offsetx -= 0.5f;
        break;
    case 's':
    case 'S':
        robot_offsety -= 0.5f;
        break;
    case 'd':
    case 'D':
        robot_offsetx += 0.5f;
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[])
{
    // Change working directory to source code path
    chdir(__FILEPATH__);
    // Initialize GLUT and GLEW, then create a window.
    ////////////////////
    glutInit(&argc, argv);
#ifdef _MSC_VER
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);
    glutCreateWindow(__FILENAME__); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
    glewInit();
#endif
    printGLContextInfo();
    My_Init();
    ////////////////////
    glutMouseFunc(My_Mouse);
    glutMotionFunc(My_MouseMotion);

    glutCreateMenu(My_Menu);
    glutAddMenuEntry("Start", 0);
    glutAddMenuEntry("Pause", 1);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // Register GLUT callback functions.
    ///////////////////////////////
    glutDisplayFunc(My_Display);
    glutReshapeFunc(My_Reshape);
    glutKeyboardFunc(My_Keyboard);
    glutTimerFunc(16, My_Timer, 0);
    ///////////////////////////////

    // Enter main event loop.
    //////////////
    glutMainLoop();
    //////////////
    return 0;
}