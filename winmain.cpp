 
#define STANDARD_EPSILON 0.000001  
#define MINIMUM_PSD 20.0
#define MINIMUM_FRAME_ENERGY 1.0  
#define PEAK_SEARCH_WIDTH 4  
#define HYSTERESIS_CONSTANT 0.2
#define FRAMES_PER_REFRESH 12  
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_image.h"
#include "portaudio.h"
#include "tables.h"
#include <stdint.h>
#include <cstring>
#include <vector>
#include "audio.h"
#include "fft1024.h"
#include "piano.h"
#include <windows.h>  
using namespace std;
#include "imgui.h"
#include "imgui_sdl.h"
#include "detect_usb.h"
typedef struct GUI_APPLICATION {
int win_w;  
int win_h;  
int pixgrid_size;  
SDL_Window * win;
SDL_Renderer * renderer;
int win_w_pixels;
int win_h_pixels;
float Tpeak;
int is_playing;  
int disposition_plot[512];
int remaining_initial_points;
int initial_modal;
audio::DeviceInfo selected_device;
float input_gain;
bool enable_playback;
int modal_showing;
char * modal_message;
int frameCount;
kwd last_key;
int refresh_devices;
} GUI_APPLICATION;
GUI_APPLICATION app;
char modal_message[1024];
void load_config(){
FILE * config_file = fopen("config.txt","r");
if(config_file){
char buffer[256];
fgets(buffer,256,config_file);
sscanf(buffer,"%f",&app.input_gain);
audio::DeviceInfo selected_device;
fgets(buffer,256,config_file);
sscanf(buffer,"%d",&selected_device.pa_id);
fgets(buffer,256,config_file);
sscanf(buffer,"%d",&selected_device.Fs);
fgets(buffer,256,config_file);
sscanf(buffer,"%f",&selected_device.latency);
fgets(buffer,256,config_file);
char name[256];
sscanf(buffer,"%s",&name);
selected_device.name=(const char*) name;
fgets(buffer,256,config_file);
sscanf(buffer,"%d",&selected_device.channels);
app.selected_device=selected_device;
fclose(config_file);
}
}
void save_config(){
FILE * config_file = fopen("config.txt","w");
fprintf(config_file,"%f\n",app.input_gain);
fprintf(config_file,"%d\n",app.selected_device.pa_id);
fprintf(config_file,"%d\n",app.selected_device.Fs);
fprintf(config_file,"%f\n",app.selected_device.latency);
fprintf(config_file,"%s\n",app.selected_device.name);
fprintf(config_file,"%d\n",app.selected_device.channels);
fclose(config_file);
}
typedef struct rgbacolor {int r; int g; int b; int a;} rgbacolor;
typedef struct pgloc {int x; int y;} pgloc;
typedef pgloc sloc;
void draw_pixel(int x, int y, int r, int g, int b, int a);
void draw_char(int x, int y, int r, int g, int b, int a, char c);
void set_color(rgbacolor col);
void draw_bitmap_char(int x, int y, int r, int g, int b, int a, int bi);
void draw_sentence(int x, int y, int r, int g, int b, int a, char * s);
int is_within_rect(SDL_Rect rect, pgloc loc) ;
void run_tuner();
void draw_disposition_plot(int next_disposition);
void modal_routine(char * message){
modal_message[0]='\0';
strcpy(modal_message,message);
app.modal_showing=1;
}
int text_width(char * sentence){
return 8*strlen(sentence);
}
int text_height(char * sentence){
return 13;
}
pgloc get_pixgrid_location(sloc loc);
pgloc _pgloc(int x, int y){  
pgloc loc = {x,y};
return loc;
}
sloc _sloc(int x, int y){  
sloc loc = {x,y};
return loc;
}
int is_within_rect(int x, int y, int w, int h, pgloc loc)  
{
SDL_Rect r={x,y,w,h};
return is_within_rect(r,loc);
}
int is_within_rect(SDL_Rect rect, pgloc loc)  
{
if((loc.x>=rect.x)&&(loc.x<=rect.x+rect.w)&&(loc.y>=rect.y)&&(loc.y<=rect.y+rect.h))
return 1;
return 0;
}
pgloc get_pixgrid_location(sloc loc){
return _pgloc(loc.x/app.pixgrid_size,loc.y/app.pixgrid_size);
}
sloc get_screen_location(pgloc loc){
return _sloc(loc.x*app.pixgrid_size,loc.y*app.pixgrid_size);
}
rgbacolor color(int r, int g, int b, int a){
rgbacolor col = {r,g,b,a};
return col;
}
SDL_Rect rectangle(int x, int y, int w, int h){
SDL_Rect rect={x,y,w,h};
return rect;
}
int get_bit(uint8_t value, int bit){
return (int)((value>>bit)&((uint8_t)0x1));
}
void set_color(rgbacolor col){
SDL_SetRenderDrawColor(app.renderer,col.r,col.g,col.b,col.a);
}
void draw_sdl_rectangle(int x, int y, int w, int h, int r, int g, int b, int a){
set_color(color(r,g,b,a));
SDL_Rect rect=rectangle(x,y,w,h);
SDL_RenderFillRect(app.renderer,&rect);
}
void draw_sentence(int x, int y, int r, int g, int b, int a, char * s){
int len = strlen(s);
for(int i=0;i<len;i++){
draw_char(x+8*i,y,r,g,b,a,s[i]);
}
}
void draw_char(int x, int y, int r, int g, int b, int a, char c){
int offs = c-32;  
uint8_t  * raster = font[offs];
for(int line=0;line<13;line++){
for(int col=0;col<8;col++){
if(get_bit(raster[12-line],7-col)){
int sx=x+col;
int sy=y+line;
draw_pixel(sx,sy,r,g,b,a);
}
}
}
}
void draw_bitmap_char(int x, int y, int r, int g, int b, int a, int bi){  
uint8_t  * raster = bitmaps[bi];
for(int line=0;line<13;line++){
for(int col=0;col<8;col++){
if(get_bit(raster[line],7-col)){
int sx=x+col;
int sy=y+line;
draw_pixel(sx,sy,r,g,b,a);
}
}
}
}
void draw_pixel(int x, int y, rgbacolor c){  
SDL_SetRenderDrawColor(app.renderer, c.r,c.g,c.b,c.a);
SDL_Rect rect;
rect.x=app.pixgrid_size*x;
rect.y=app.pixgrid_size*y;
rect.w=app.pixgrid_size;
rect.h=app.pixgrid_size;
SDL_RenderFillRect(app.renderer,&rect);
}
void draw_pixel(int x, int y, int r, int g, int b, int a){  
rgbacolor c = {r,g,b,a};
draw_pixel(x,y,c);
}
void snap_window_size_to_pixgrid(){
int wsp = app.win_w;
int hsp= app.win_h;
int wspadj = app.pixgrid_size*(wsp/app.pixgrid_size);
int hspadj = app.pixgrid_size*(hsp/app.pixgrid_size);
app.win_w_pixels=wspadj/app.pixgrid_size;
app.win_h_pixels=hspadj/app.pixgrid_size;
if(wspadj!=wsp||hspadj!=hsp){
app.win_w=wspadj;
app.win_h=hspadj;
}
}
void init_ui(){
ImGui::CreateContext();
	ImGuiSDL::Initialize(app.renderer, app.win_w, app.win_h);
}
void construct_ui(){
int refresh = ((app.frameCount%FRAMES_PER_REFRESH)==0);
ImGui::NewFrame();
bool p_open = true;
bool auto_resize=true;
ImGuiWindowFlags flags = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : 0;
flags|=ImGuiWindowFlags_NoMove;
flags|=ImGuiWindowFlags_NoResize;
flags|=ImGuiWindowFlags_NoCollapse;
ImGui::SetNextWindowPos(ImVec2(20,20), ImGuiCond_Always);
ImGui::Begin(" ",&p_open,flags);
if(!app.is_playing){
ImGui::Text("Input Device:");
std::vector<audio::DeviceInfo>& input_devices=audio::list_input_devices(app.refresh_devices);
audio::DeviceInfo rocksmith_device=audio::find_input_device();
for(int i=0;i<input_devices.size();i++){
audio::DeviceInfo device = input_devices[i];
if(rocksmith_device.pa_id!=-1)
if((strcmp(device.name,rocksmith_device.name)==0)&&app.selected_device.pa_id==-1){
app.selected_device=device;
}
char label[256];
sprintf(label,"%s (%dHz)",device.name,device.Fs);
if(ImGui::RadioButton(label,device.pa_id==app.selected_device.pa_id))
{
app.selected_device=device;
}
}
char label[256];
audio::DeviceInfo playback_device=audio::find_output_device(app.refresh_devices);
app.refresh_devices=0;
sprintf(label,"%s (%dHz)",playback_device.name,playback_device.Fs);
ImGui::Text("\nOutput Device:");
ImGui::Text(label);
ImGui::Text("\nInput Gain:");
ImGui::SliderFloat("", &app.input_gain, 1.0, 10.0, "%.2fX");
ImGui::Text("");
if(ImGui::Button("Start Tuner")){
app.is_playing=(audio::play(app.selected_device,app.input_gain,app.enable_playback)==0);
}
}else{
if(ImGui::Button("Stop Tuner")){
int trystop=audio::stop();
if(trystop==0){
app.is_playing=0;
app.modal_showing=0;
app.refresh_devices = 1;
}
}
}
if(app.modal_showing){
ImGui::Text("");
ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),app.modal_message);
}
ImGui::End();  
}
void gui_application_draw_loop(){
construct_ui();
run_tuner();
ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
}
void destroy_ui(){
ImGuiSDL::Deinitialize();
ImGui::DestroyContext();
}
int event_loop(){
ImGuiIO& io = ImGui::GetIO();
int wheel = 0;
int mouseX,mouseY;
const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
		 
		
		io.DeltaTime = 1.0f / 60.0f;
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		io.MouseWheel = static_cast<float>(wheel);
SDL_Event event;
while (SDL_PollEvent(&event))
{
switch (event.type)
{
case SDL_QUIT:
return 0;
break;
case SDL_WINDOWEVENT:
if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
{
io.DisplaySize.x = static_cast<float>(event.window.data1);
io.DisplaySize.y = static_cast<float>(event.window.data2);
}
break;
case SDL_MOUSEBUTTONDOWN:
break;
case SDL_MOUSEBUTTONUP:
break;
default:
break;
}
}
return 1;
}
void draw_disposition_plot(int next_disposition){
for(int i=0;i<512-1;i++){
app.disposition_plot[i]=app.disposition_plot[i+1];
}
int dis = (int)((float)next_disposition*HYSTERESIS_CONSTANT+(float)app.disposition_plot[510]*(1.0-HYSTERESIS_CONSTANT));
app.disposition_plot[511]=dis;
for(int i=0;i<512;i++){
int x = i;
int y = 255-app.disposition_plot[i];
draw_pixel(x/app.pixgrid_size,255/app.pixgrid_size,0,0,255,255);  
draw_pixel(x/app.pixgrid_size,(y)/app.pixgrid_size,0,255,0,255);
}
if(app.remaining_initial_points>0)
app.remaining_initial_points--;
}
void run_tuner(){
if(!app.is_playing)
return;
float * table = audio::get_table();
float frame_energy = 0.0;
for(int i=0;i<1024;i++){
frame_energy+=table[i]*table[i];
}
if(frame_energy<(MINIMUM_FRAME_ENERGY*2.0)){
app.last_key=piano_null_key();
draw_disposition_plot(0);
}else{
float ac[1024];
for(int k=0;k<1024;k++){
float total = 0.0;
for(int n=0;n<1024-k;n++){
total+=table[n]*table[n+k];
}
total/=(float)(1024-k);
ac[k]=total;
}
float ac0=ac[0];
if(fabs(ac0)<STANDARD_EPSILON){
return;
}
for(int i=0;i<1024;i++){
ac[i]=ac[i]/ac0;
}
COMPLEX x[1024];
COMPLEX X[1024];
real_buffer_to_complex_buffer(ac,x);
fft(x,X);
float psd[1024];
complex_buffer_realpart_to_real_buffer(X,psd);
for(int i=0;i<512;i++){
if(fabs(psd[i])<MINIMUM_PSD){
psd[i]=0.0;
}else{
}
}
float m = psd[0];
int argmax = 0;
for(int i=1;i<512;i++){  
if(psd[i]>m){
m=psd[i];
argmax=i;
}
}
int peak = -1;
for(int k=0;k<512-2*PEAK_SEARCH_WIDTH;k++){
float a = psd[k];
float b = psd[k+PEAK_SEARCH_WIDTH];
float c = psd[k+2*PEAK_SEARCH_WIDTH];
if(b>a && b>c)
{
peak=k+PEAK_SEARCH_WIDTH;
break;
}
}
if(peak==-1)
return;
float bin_metric = 0.0;
float total=0.0;
for(int k=peak-PEAK_SEARCH_WIDTH;k<peak+PEAK_SEARCH_WIDTH;k++){
total+=psd[k];
}
for(int k=peak-PEAK_SEARCH_WIDTH;k<peak+PEAK_SEARCH_WIDTH;k++){
bin_metric+=(float)k*psd[k]/total;
}
float Fseff = audio::get_sample_rate()/audio::get_down_sampling();
float f0=bin_metric/1024.0*Fseff;
kwd nearest_key=piano_find_key(f0);
if(app.last_key.key!=-1&&nearest_key.key!=-1&&(nearest_key.key-app.last_key.key==11))
{
nearest_key=app.last_key;
}
app.last_key=nearest_key;
int next_disposition;
if(nearest_key.key!=-1){
next_disposition=nearest_key.disposition;
}
else{
next_disposition=app.disposition_plot[511];
}
draw_disposition_plot(next_disposition);
if(nearest_key.key!=-1){
char descriptor[64];
piano_get_descriptor_no_disposition(nearest_key,descriptor);
int topleft_x=app.win_w_pixels/2-text_width(descriptor)/2;
int topleft_y=app.win_h_pixels/2-text_height(descriptor)/2;
draw_sentence(topleft_x,topleft_y,255,255,255,255,descriptor);
}
}
}
void device_refresh_procedure(){
printf("Refreshing\n");
app.selected_device=audio::null_device();
if(!app.is_playing)
app.refresh_devices=1;
}
void run_gui_application(){
int rendererFlags, windowFlags;
rendererFlags = SDL_RENDERER_ACCELERATED;
rendererFlags=0;
windowFlags=0;
app.win_w=512;
app.win_h=512;
app.pixgrid_size = 4;  
app.Tpeak=0.08;
app.is_playing=0;
for(int i=0;i<512;i++){
app.disposition_plot[i]=0;
}
app.remaining_initial_points=512;
app.initial_modal=1;  
app.selected_device=audio::null_device();
app.input_gain=1.0;
app.enable_playback=true;
app.modal_showing=0;
app.modal_message = modal_message;
app.frameCount=0;
app.last_key=piano_null_key();
app.refresh_devices=1;
SDL_Init(SDL_INIT_VIDEO);
SDL_Window * win = SDL_CreateWindow("GuitarTunerWin",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,app.pixgrid_size*(app.win_w/app.pixgrid_size),app.pixgrid_size*(app.win_h/app.pixgrid_size),windowFlags);
snap_window_size_to_pixgrid();
SDL_Surface * icon=IMG_Load("icon.png");
SDL_SetWindowIcon(win,icon);
app.win=win;
SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
SDL_Renderer * renderer = SDL_CreateRenderer(win, -1, rendererFlags);
app.renderer=renderer;
init_ui();
DetectUSB::init(&device_refresh_procedure);
int running = 1;
while(running){
SDL_Delay(16);  
running=event_loop();
draw_sdl_rectangle(0,0,app.win_w,app.win_h,0,0,0,255);
gui_application_draw_loop();
SDL_RenderPresent(renderer);
app.frameCount++;
if(app.frameCount==60)
app.frameCount=0;
}
SDL_DestroyWindow(win);
SDL_Quit();
destroy_ui();
DetectUSB::terminate();
}
int entry_point(){
audio::init(&modal_routine);
run_gui_application();
audio::terminate();
return 0;
}
#ifdef BUILD_RELEASE
int APIENTRY WinMain(HINSTANCE instance,HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow)
{
return entry_point();
}
#endif
#ifdef BUILD_DEBUG
int main(){
return entry_point();
}
#endif
