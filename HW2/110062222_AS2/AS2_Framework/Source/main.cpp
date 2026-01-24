#include "../Externals/Include/Common.h"

#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3

using namespace glm;
using namespace std;

float splitPosition = 0.5f;
bool isDragging = false;
const float handleRadius = 0.02f;

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;
GLuint programID;
GLuint mvUniform;
GLuint pUniform;
GLuint modeUniform;
GLuint timeUniform;

int current_mode = 7;
int windowWidth, windowHeight;
// mouse 
bool mouse_pressed = false;
int mouse_x = 0, mouse_y = 0;
float rotation_x = 0.0f;
float rotation_y = 0.0f;
vec3 eye_position = vec3(0.0f, -100.0f, -30.0f);
vec3 front_vector = vec3(0.0f, 0.0f, -1.0f);  
vec3 right_vector = vec3(1.0f, 0.0f, 0.0f); 
vec3 up_vector = vec3(0.0f, 1.0f, 0.0f);

struct Shape
{
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	int drawCount;
	int materialID;
};

struct Material
{
	GLuint diffuse_tex;
};

std::vector<Shape> shapes; 
std::vector<Material> materials;

void updateVectors() {
	front_vector.x = sin(rotation_y) * cos(rotation_x);
	front_vector.y = -sin(rotation_x);
	front_vector.z = -cos(rotation_y) * cos(rotation_x);
	front_vector = normalize(front_vector);

	right_vector = normalize(cross(front_vector, vec3(0.0f, 1.0f, 0.0f)));
	up_vector = normalize(cross(right_vector, front_vector));
}

void freeShaderSource(char** srcp)
{
	delete[] srcp[0];
	delete[] srcp;
}

char** loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char** srcp = new char* [1];
	srcp[0] = src;
	return srcp;
}


GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	char** vertexSource = loadShaderSource(vertex_file_path);
	char** fragmentSource = loadShaderSource(fragment_file_path);

	glShaderSource(VertexShaderID, 1, vertexSource, NULL);
	glCompileShader(VertexShaderID);
	freeShaderSource(vertexSource);

	glShaderSource(FragmentShaderID, 1, fragmentSource, NULL);
	glCompileShader(FragmentShaderID);
	freeShaderSource(fragmentSource);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, VertexShaderID);
	glAttachShader(programID, FragmentShaderID);
	glLinkProgram(programID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	return programID;
}

GLuint noiseImage;
GLuint fbo;
GLuint rbo;                     
GLuint colorTexture;            
GLuint postProcessingProgram;   
GLuint quadVAO, quadVBO;      

void initFramebuffer() {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 600, 600, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 600, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	postProcessingProgram = LoadShaders("../Assets/post_vertex.glsl", "../Assets/post_fragment.glsl");
}

void My_Init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	programID = LoadShaders("../Assets/vertex.vs.glsl", "../Assets/fragment.fs.glsl");
	const aiScene* scene = aiImportFile("../sponza/sponza.obj", aiProcessPreset_TargetRealtime_MaxQuality);

	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		Material mat; 
		aiString texturePath; 

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{
			char newPath[256];
			snprintf(newPath, sizeof(newPath), "../sponza/%s", texturePath.C_Str());
			texture_data img_data = loadImg(newPath);

			if (img_data.data)
			{
				glGenTextures(1, &mat.diffuse_tex);
				glBindTexture(GL_TEXTURE_2D, mat.diffuse_tex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img_data.width, img_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data.data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glGenerateMipmap(GL_TEXTURE_2D);
				free(img_data.data); 
			}
			else
			{
				printf("Failed to load texture: %s\n", texturePath.C_Str());
			}
		}

		materials.push_back(mat); 
	}
	texture_data img_data = loadImg("../Assets/noise.png");
	if (img_data.data)
	{
		glGenTextures(1, &noiseImage);
		glBindTexture(GL_TEXTURE_2D, noiseImage);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img_data.width, img_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(img_data.data);
	}
	else
	{
		printf("Failed to load texture: noise\n");
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];

		Shape shape;
		glGenVertexArrays(1, &shape.vao);
		glBindVertexArray(shape.vao);

		glGenBuffers(1, &shape.vbo_position);
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), &mesh->mVertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		if (mesh->HasNormals()) {
			glGenBuffers(1, &shape.vbo_normal);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), &mesh->mNormals[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(2);
		}

		if (mesh->HasTextureCoords(0)) {
			glGenBuffers(1, &shape.vbo_texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), &mesh->mTextureCoords[0][0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(1);
		}

		glGenBuffers(1, &shape.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
		std::vector<GLuint> indices;
		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; k++) {
				indices.push_back(face.mIndices[k]);
			}
		}
		shape.drawCount = indices.size();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.drawCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		shape.materialID = mesh->mMaterialIndex;
		shapes.push_back(shape); 
	}

	mvUniform = glGetUniformLocation(programID, "um4mv");
	pUniform = glGetUniformLocation(programID, "um4p");
	

	aiReleaseImport(scene);
}

void My_Display() {
	windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateVectors();
	glUseProgram(programID);

	mat4 mv_matrix = mat4(1.0f);
	mv_matrix = rotate(mv_matrix, rotation_x, vec3(1.0f, 0.0f, 0.0f));
	mv_matrix = rotate(mv_matrix, rotation_y, vec3(0.0f, 1.0f, 0.0f));
	mv_matrix = translate(mv_matrix, vec3(0, -100, -30.0f));
	mv_matrix = translate(mv_matrix, -eye_position);

	mat4 projection = perspective(radians(45.0f), 1.0f, 0.1f, 3000.0f);

	glUniformMatrix4fv(mvUniform, 1, GL_FALSE, value_ptr(mv_matrix));
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, value_ptr(projection));
	glUniform1i(glGetUniformLocation(programID, "mode"), current_mode);
	

	for (const Shape& shape : shapes) {
		glBindVertexArray(shape.vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, materials[shape.materialID].diffuse_tex);
		glDrawElements(GL_TRIANGLES, shape.drawCount, GL_UNSIGNED_INT, nullptr);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(postProcessingProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glUniform1i(glGetUniformLocation(postProcessingProgram, "screenTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseImage);
	glUniform1i(glGetUniformLocation(postProcessingProgram, "noiseTexture"), 0);

	float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	glUniform1f(glGetUniformLocation(postProcessingProgram, "time") , currentTime);
	glUniform1i(glGetUniformLocation(postProcessingProgram, "mode"), current_mode);
	glUniform2f(glGetUniformLocation(postProcessingProgram, "mousePos"), mouse_x/(float)windowWidth, 1-mouse_y/(float)windowHeight);


	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;

	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(timer_speed, My_Timer, val);
}


void My_MouseMotion(int x, int y)
{
	
	float tmp_x = x / (float)windowWidth;
	float tmp_y = y / (float)windowHeight;
	float distToSplit = abs(tmp_x - splitPosition);
	float distToHandle = sqrt(pow(tmp_x - splitPosition, 2) +
		pow(tmp_y - 0.5f, 2));

	if ((distToHandle < handleRadius || isDragging) && tmp_x >= 0.0 && tmp_x <= 1.0) {
		isDragging = true;
		splitPosition = tmp_x;
	}

	glUniform1i(glGetUniformLocation(postProcessingProgram, "isDragging"), isDragging);
	glUniform1f(glGetUniformLocation(postProcessingProgram, "splitPosition"), splitPosition);

	if (mouse_pressed && !isDragging)
	{
		int dx = x - mouse_x;
		int dy = y - mouse_y;

		rotation_y += dx * 0.005f;
		rotation_x += dy * 0.005f;

		if (rotation_x > 1.5f) rotation_x = 1.5f;
		if (rotation_x < -1.5f) rotation_x = -1.5f;

		glutPostRedisplay();
	}

	mouse_x = x;
	mouse_y = y;
}

void My_Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouse_pressed = true;
			mouse_x = x;
			mouse_y = y;
		}
		else if (state == GLUT_UP)
		{
			mouse_pressed = false;
			isDragging = false;
		}
	}

}

void My_PassiveMotion(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}

void My_Keyboard(unsigned char key, int x, int y) {
	float moveSpeed = 5.0f;  

	switch (key) {
	case 'w': 
		eye_position += front_vector * moveSpeed;
		break;
	case 's':  
		eye_position -= front_vector * moveSpeed;
		break;
	case 'a':  
		eye_position -= right_vector * moveSpeed;
		break;
	case 'd': 
		eye_position += right_vector * moveSpeed;
		break;
	case 'z':
		eye_position += up_vector * moveSpeed;
		break;
	case 'x':  
		eye_position -= up_vector * moveSpeed;
		break;
	}

	glutPostRedisplay();
}



void My_SpecialKeys(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}



void My_Menu(int id)
{
	current_mode = id;
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
    // Change working directory to source code path
    chdir(__FILEPATH__("/../Assets/"));
#endif
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
	glutCreateWindow("110062222_AS2"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();
	initFramebuffer();
	// Create a menu and bind it to mouse right button.
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddMenuEntry("Normal", 0);
	glutAddMenuEntry("Image Abstraction", 1);
	glutAddMenuEntry("Watercolor", 2);
	glutAddMenuEntry("Magnifier", 3);
	glutAddMenuEntry("Bloom Effect", 4);
	glutAddMenuEntry("Pixelization", 5);
	glutAddMenuEntry("Sine wave distortion", 6);
	glutAddMenuEntry("General", 7);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutMotionFunc(My_MouseMotion);
	glutPassiveMotionFunc(My_PassiveMotion);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0); 

	// Enter main event loop.
	glutMainLoop();

	return 0;
}
