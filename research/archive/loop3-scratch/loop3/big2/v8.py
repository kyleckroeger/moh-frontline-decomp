A='inline CVector3 operator*(float k, const CVector3& v) { return CVector3(k * v.x, k * v.y, k * v.z); }\n'
B='inline CVector3 operator*(const CVector3& v, float k) { return CVector3(v.x * k, v.y * k, v.z * k); }\n'
B2='inline CVector3 operator*(const CVector3& v, float k) { return CVector3(k * v.x, k * v.y, k * v.z); }\n'
R='''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
'''
def r(c,b): return '                    if (%s < 0.0f) {\n                        flat += %s;\n'%(c,b)
V=[
('mix1',[(R,r('front.Dot(flat)','(flat.Dot(front) * -2.0f) * front'))]),
('mix2',[(R,r('flat.Dot(front)','(front.Dot(flat) * -2.0f) * front'))]),
('mix3',[(R,r('front.Dot(flat)','(front.Dot(flat) * -2.0f) * front'))]),
('vk1',[(A,A+B),(R,r('flat.Dot(front)','front * (flat.Dot(front) * -2.0f)'))]),
('vk2',[(A,A+B2),(R,r('flat.Dot(front)','front * (flat.Dot(front) * -2.0f)'))]),
('vk3',[(A,A+B2),(R,r('flat.Dot(front)','front * (-2.0f * flat.Dot(front))'))]),
('m2a',[(R,r('flat.Dot(front)','(-2.0f * flat.Dot(front)) * front'))]),
('m2b',[(R,r('flat.Dot(front)','-2.0f * flat.Dot(front) * front'))]),
('m2c',[(R,r('flat.Dot(front)','flat.Dot(front) * -2.0f * front'))]),
]
