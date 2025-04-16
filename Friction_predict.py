import joblib
import numpy as np

# 모델과 인코더 불러오기
model = joblib.load("friction.pkl")
road_encoder = joblib.load("road.pkl")
cond_encoder = joblib.load("cond.pkl")

def predict_mu(road, cond, slope, speed, weight):
    road_encoded = road_encoder.transform([road])[0]
    cond_encoded = cond_encoder.transform([cond])[0]

    X_input = np.array([[road_encoded, cond_encoded, slope, speed, weight]])
    mu = model.predict(X_input)[0]
    return round(mu, 4)

# 예시:
mu = predict_mu("아스팔트", "건조", 5, 60, 1300)
print(f"예측된 마찰계수: μ = {mu}")
