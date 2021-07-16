#ifndef PIANO_H
#define PIANO_H
#include <cmath>
const float keys_f[88] = {
27.5000,29.1352,30.8677,32.7032,34.6478,36.7081,38.8909,41.2034,43.6535,46.2493,48.9994,
51.9131,55.0000,58.2705,61.7354,65.4064,69.2957,73.4162,77.7817,82.4069,87.3071,92.4986,
97.9989,103.8262,110.0000,116.5409,123.4708,130.8128,138.5913,146.8324,155.5635,164.8138,174.6141,
184.9972,195.9977,207.6523,220.0000,233.0819,246.9417,261.6256,277.1826,293.6648,311.1270,329.6276,
349.2282,369.9944,391.9954,415.3047,440.0000,466.1638,493.8833,523.2511,554.3653,587.3295,622.2540,
659.2551,698.4565,739.9888,783.9909,830.6094,880.0000,932.3275,987.7666,1046.5023,1108.7305,1174.6591,
1244.5079,1318.5102,1396.9129,1479.9777,1567.9817,1661.2188,1760.0000,1864.6550,1975.5332,2093.0045,2217.4610,
2349.3181,2489.0159,2637.0205,2793.8259,2959.9554,3135.9635,3322.4376,3520.0000,3729.3101,3951.0664,4186.0090,
};
const char* keys_s[88] = {
"A0","A#0","B0",
"C1","C#1","D1","D#1","E1","F1","F#1","G1","G#1","A1","A#1","B1",
"C2","C#2","D2","D#2","E2","F2","F#2","G2","G#2","A2","A#2","B2",
"C3","C#3","D3","D#3","E3","F3","F#3","G3","G#3","A3","A#3","B3",
"C4","C#4","D4","D#4","E4","F4","F#4","G4","G#4","A4","A#4","B4",
"C5","C#5","D5","D#5","E5","F5","F#5","G5","G#5","A5","A#5","B5",
"C6","C#6","D6","D#6","E6","F6","F#6","G6","G#6","A6","A#6","B6",
"C7","C#7","D7","D#7","E7","F7","F#7","G7","G#7","A7","A#7","B7",
"C8",
};
typedef struct kwd {int key; int disposition;} kwd;  
#define PIANO_COB 0.05776226504  
kwd piano_find_key(float f0){ 
kwd k;
k.key=-1;
k.disposition=0;
if(f0<0.000001)
return k;
int key_above = -1;
int key_below=0;
for(int i=1;i<88;i++){
if(keys_f[i]>f0)
{
key_above=i;
key_below=i-1;
break;
}
}
if(key_above!=-1){
float fu = keys_f[key_above];
float fb = keys_f[key_below];
float delta_fu=abs(log(fu/27.5)-log(f0/27.5));
float delta_fb=abs(log(fb/27.5)-log(f0/27.5));
int tk = key_below;
if(delta_fu<delta_fb){
tk=key_above;
}
float tf=keys_f[tk];
k.key=tk;
k.disposition=(int)trunc(log(f0/tf)/PIANO_COB*100.0);
}
return k;
}
void piano_get_descriptor(kwd key, char * descriptor){
sprintf(descriptor,"%s%+2d",keys_s[key.key],key.disposition);
}
void piano_get_descriptor_no_disposition(kwd key, char * descriptor){
sprintf(descriptor,"%s",keys_s[key.key]);
}
kwd piano_null_key(){
kwd nk;
nk.key=-1;
nk.disposition=0;
return nk;
}
#endif