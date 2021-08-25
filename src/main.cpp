#include <vector>
#include <cmath>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// probably temporary
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include <iostream>

#define SQ(a) (a * a)

struct point
{
  float x, y, z;

  static float distance(const point& a, const point& b)
  {
    return (float)sqrt(pow(a.x - b.x, 2.0) + pow(a.y - b.y, 2.0) + pow(a.z - b.z, 2.0));
  }
};

struct triangle
{
  point p1, p2, p3;
};

inline float rand_color()
{
  return (float) rand() / (float) RAND_MAX;
}

bool float_eq(float a, float b)
{
  if (fabs(a - b) < 0.001) return true;
  return false;
}

// Ok I'll implement a 3D triangulation
triangle gen_triangle(const point& pivot, const std::vector<point>& point_set)
{
  triangle tri;
  
  // Search for the 2 closest points
  float dist = INFINITY;
  point closest_point, sec_closest_point;

  // First one
  for (const point& a : point_set)
  {
    //printf("a: %f %f %f\n", a.x, a.y, a.z);

    // Check if we're not on the pivot
    if (float_eq(a.x, pivot.x) &&
        float_eq(a.y, pivot.y) &&
        float_eq(a.z, pivot.z)) continue;

    const float d = point::distance(a, pivot);

    if (d < dist)
    {
      dist = d;
      closest_point = a;
    }
  }

  dist = INFINITY;

  // Second one
  for (const point& a : point_set)
  {
    // Check if we're not on the pivot or the closest point
    if ((float_eq(a.x, pivot.x) &&
        float_eq(a.y, pivot.y) &&
        float_eq(a.z, pivot.z)) ||
        (float_eq(a.x, closest_point.x) &&
        float_eq(a.y, closest_point.y) &&
        float_eq(a.z, closest_point.z))) continue;

    const float d = point::distance(a, pivot);
    //printf("Dist: %f\n", d);

    if (d < dist)
    {
      dist = d;
      sec_closest_point = a;
    }
  }

  tri.p1 = pivot;
  tri.p2 = closest_point;
  tri.p3 = sec_closest_point;

  printf("Generated triangle: %f %f %f - %f %f %f - %f %f %f\n", pivot.x,
          pivot.y,
          pivot.z,
          closest_point.x,
          closest_point.y,
          closest_point.z,
          sec_closest_point.x,
          sec_closest_point.y,
          sec_closest_point.z);

  return tri;
}

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 800;

float dx,
dy, dz; // Position of the camera
const float MOV_SPEED = 0.01f;

float lastX = 400, lastY = 300;
float pitch = 0.0f, yaw = 0.0f;
float part_pitch = 0.0f, part_yaw = 0.0f;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

#define MOVE_SPEED 1
#define ANGLE_SPEED 0.06
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    const float sensitivity = 0.05f;
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Translation.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
      cameraPos.x -= xoffset;
      cameraPos.y -= yoffset;
    }

    // Rotation.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
      part_yaw   -= xoffset * 0.2;
      part_pitch += yoffset * 0.2;
    }
}

void processInput(GLFWwindow *window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

  else if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  else if(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dy += MOV_SPEED;
  
  else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dx -= MOV_SPEED;
  
  else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dy -= MOV_SPEED;
  
  else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dx += MOV_SPEED;

  else if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) dz -= MOV_SPEED;

  else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) dz += MOV_SPEED;
}

GLFWwindow* init_open_gl()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  //WINDOW CREATION
  GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "vape", NULL, NULL);

  if (window == NULL)
  {
    std::cout << "Failed to initialize GLFW" << std::endl;
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return NULL;
  }

  //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetCursorPosCallback(window, mouse_callback);
  //glfwSetScrollCallback(window, scroll_callback); 
  glfwSwapInterval(0);

  const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
  const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model

  printf("%s %s\n", vendor, renderer);    

  //So triagles do not overlap improperly.
  glEnable(GL_DEPTH_TEST);

  return window;
}

void generate_surface(std::vector<triangle> &surface)
{
  // Generate terrain
  int rows = 10;
  int cols = 10;

  std::vector<point> point_vec;
  for (int i = 0; i < rows ; i++)
    for (int j = 0; j < cols ; j++)
    {
      point a = {(float)i * 0.1f, (float)j * 0.1f, rand_color()};
      printf("Putting: %f %f %f\n", a.x, a.y, a.z);
      point_vec.push_back(a);
    }

  // ill begin with an ugly algorithm that makes a triangle out of the 3 closest.
  for (const point& a : point_vec)
  {
    surface.push_back(gen_triangle(a, point_vec));
  }
}

struct render_object_t
{
  unsigned int vao, vbo;
  shader_t *shader_program;
  unsigned int shader_program_id;
  unsigned int triangle_n = 0;

  render_object_t(std::vector<triangle> surface)
  {
    // Gen buffers
    std::vector<float> vtx_buffer;
    for (const triangle& a : surface)
    {
      const float r = rand_color();
      const float g = rand_color();
      const float b = rand_color();

      vtx_buffer.push_back(a.p1.x);
      vtx_buffer.push_back(a.p1.y);
      vtx_buffer.push_back(a.p1.z);
      vtx_buffer.push_back(r);
      vtx_buffer.push_back(g);
      vtx_buffer.push_back(b);

      vtx_buffer.push_back(a.p2.x);
      vtx_buffer.push_back(a.p2.y);
      vtx_buffer.push_back(a.p2.z);
      vtx_buffer.push_back(r);
      vtx_buffer.push_back(g);
      vtx_buffer.push_back(b);

      vtx_buffer.push_back(a.p3.x);
      vtx_buffer.push_back(a.p3.y);
      vtx_buffer.push_back(a.p3.z);
      vtx_buffer.push_back(r);
      vtx_buffer.push_back(g);
      vtx_buffer.push_back(b);

      triangle_n++;
    }

    // Buffers and shader
    shader_err s_err;
    shader_program = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl", &s_err);

    //TODO: error check here!
    shader_program_id = shader_get_id(shader_program);

    //Init stuff
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
            vtx_buffer.size() * sizeof(float),
            &vtx_buffer[0],
            GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
  }

  void render()
  {
    // draw
    use_shader(shader_program);
    glBindVertexArray(vao);

    // Apply transformations
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(dx, dy, dz));
    model = glm::rotate(model, part_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, part_pitch, glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(20.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

    unsigned int model_loc = glGetUniformLocation(shader_program_id, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

    unsigned int view_loc = glGetUniformLocation(shader_program_id, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

    unsigned int projection_loc = glGetUniformLocation(shader_program_id, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, triangle_n * 3);
  }

  ~render_object_t()
  {
    unload_shader(shader_program);
  }
};

struct reference_arrows_t
{
  unsigned int vao, vbo;
  shader_t *shader_program;
  unsigned int shader_program_id;
  unsigned int triangle_n = 0;

  reference_arrows_t()
  {
    float data[] = {
      // Layout: x y z - r g b
      0.0, 0.0, 0.0, 1.0, 0.0, 0.0, // X direction
      1.0, 0.0, 0.0, 1.0, 0.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 1.0, 0.0, // Y direction
      0.0, 1.0, 0.0, 0.0, 1.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 0.0, 1.0, // Z direction
      0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
    };

    // Buffers and shader
    shader_err s_err;
    shader_program = load_shader("shaders/ref_line_vertex.glsl", "shaders/ref_line_fragment.glsl", &s_err);

    //TODO: error check here!
    shader_program_id = shader_get_id(shader_program);

    //Init stuff
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(data),
            data,
            GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
  }

  ~reference_arrows_t()
  {
    unload_shader(shader_program);
  }

  void render()
  {
    // draw
    use_shader(shader_program);
    glBindVertexArray(vao);

    glm::mat4 mvp_trans;

    // Apply transformations
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(dx, dy, dz));
    //model = glm::translate(model, glm::vec3(0.0, 0.0, -5.0));
    model = glm::rotate(model, part_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, part_pitch, glm::vec3(1.0f, 0.0f, 0.0f));

    //glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    //glm::mat4 projection = glm::perspective(glm::radians(20.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

    //unsigned int model_loc = glGetUniformLocation(shader_program_id, "model");
    //glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

    //unsigned int view_loc = glGetUniformLocation(shader_program_id, "view");
    //glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

    //unsigned int projection_loc = glGetUniformLocation(shader_program_id, "projection");
    //glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    mvp_trans = model;

    unsigned int mvp_trans_loc = glGetUniformLocation(shader_program_id, "mvp_trans");
    glUniformMatrix4fv(mvp_trans_loc, 1, GL_FALSE, glm::value_ptr(mvp_trans));

    glDrawArrays(GL_LINES, 0, 6);
  }
};

int main()
{
  GLFWwindow *window = init_open_gl();

  std::vector<triangle> surface;
  generate_surface(surface);

  render_object_t surface_renderer(surface);
  reference_arrows_t ref_arrows;

  std::cout << "Starting to render!\n";
  while(!glfwWindowShouldClose(window))
  {
    processInput(window);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ref_arrows.render();
    surface_renderer.render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}

