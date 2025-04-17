import pandas as pd
import joblib
import numpy as np
import os

# 기존 모델 로드
model_old = joblib.load("friction.pkl")
road_enc_old = joblib.load("road.pkl")
cond_enc_old = joblib.load("cond.pkl")


# 조건별 분할 모델 예측 함수 (도로조건,환경조건에 따라 분할)
def predict_group_model(road, cond, slope, speed, weight):
    key = f"{road}/{cond}".replace("/", "_")
    try:
        model = joblib.load(f"group_models/model_{key}.pkl")
        road_enc = joblib.load(f"group_models/road_enc_{key}.pkl")
        cond_enc = joblib.load(f"group_models/cond_enc_{key}.pkl")
    except FileNotFoundError:
        return None

    try:
        road_enc_val = road_enc.transform([road])[0]
        cond_enc_val = cond_enc.transform([cond])[0]
    except:
        return None

    speed_weight = speed / weight if weight != 0 else 0
    slope_sq = slope**2

    X = np.array([[road_enc_val, cond_enc_val, slope, speed, weight]])
    return model.predict(X)[0]


# 기존 모델 예측 함수
def predict_old_model(row):
    try:
        road = road_enc_old.transform([row["도로조건"]])[0]
        cond = cond_enc_old.transform([row["환경조건"]])[0]
        X = np.array(
            [[road, cond, row["경사도(%)"], row["차량속도(km/h)"], row["차량중량(kg)"]]]
        )
        return model_old.predict(X)[0]
    except:
        print(f"❌ 기존모델 인코딩 실패: {row['도로조건']} / {row['환경조건']}")
        return None


# 오차 계산 함수
def rel_error(true, pred):
    return abs(true - pred) / true * 100 if true != 0 else 0


df = pd.read_csv("250data.csv")
selected_mu = []
model_choice = []
old_predictions = []
new_predictions = []
old_errors = []
new_errors = []

# 예측 비교
for _, row in df.iterrows():
    true_mu = row["마찰계수(μ)"]

    mu_old = predict_old_model(row)
    mu_new = predict_group_model(
        row["도로조건"],
        row["환경조건"],
        row["경사도(%)"],
        row["차량속도(km/h)"],
        row["차량중량(kg)"],
    )

    old_predictions.append(mu_old)
    new_predictions.append(mu_new)

    if mu_old is None or mu_new is None:
        selected_mu.append(None)
        model_choice.append("Error")
        old_errors.append(None)
        new_errors.append(None)
        continue

    err_old = rel_error(true_mu, mu_old)
    err_new = rel_error(true_mu, mu_new)

    if err_old <= err_new:
        selected_mu.append(mu_old)
        model_choice.append("기존모델")
    else:
        selected_mu.append(mu_new)
        model_choice.append("조건분할모델")

    old_errors.append(round(err_old, 2))
    new_errors.append(round(err_new, 2))

# 결과 컬럼 저장
df["기존 예측"] = old_predictions
df["조건분할 예측"] = new_predictions
df["기존 오차(%)"] = old_errors
df["조건분할 오차(%)"] = new_errors
df["선택된 예측값"] = selected_mu
df["선택 모델"] = model_choice

# 저장
df.to_csv("기존 모델 vs 조건분할 모델.csv", index=False)
