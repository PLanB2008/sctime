/** Rundet f auf das naechste Vielfache von step*/
float roundTo(float f, float step)
{
  return int(f/step+0.5)*step;
}