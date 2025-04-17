import os
import pandas as pd
import joblib
from xgboost import XGBRegressor
from sklearn.ensemble import RandomForestRegressor
from sklearn.preprocessing import LabelEncoder

# 조건별 모델 학습

# 데이터 로드
df = pd.read_csv("250data.csv")

# 조건 키 생성
df["key"] = df["도로조건"] + "/" + df["환경조건"]

# 저장 경로 준비
os.makedirs("group_models", exist_ok=True)

# 조건별 모델 학습
for key, group in df.groupby("key"):
    if len(group) < 10:
        continue
    group = group.copy()

    # 인코딩 준비
    road_enc = LabelEncoder()
    cond_enc = LabelEncoder()
    group["도로조건"] = road_enc.fit_transform(group["도로조건"])
    group["환경조건"] = cond_enc.fit_transform(group["환경조건"])

    X = group[["도로조건", "환경조건", "경사도(%)", "차량속도(km/h)", "차량중량(kg)"]]
    y = group["마찰계수(μ)"]

    # 모델 학습
    model = XGBRegressor(n_estimators=50, learning_rate=0.1, random_state=42)
    model.fit(X, y)

    # 모델 및 인코더 저장
    safe_key = key.replace("/", "_")
    joblib.dump(model, f"group_models/model_{safe_key}.pkl")
    joblib.dump(road_enc, f"group_models/road_enc_{safe_key}.pkl")
    joblib.dump(cond_enc, f"group_models/cond_enc_{safe_key}.pkl")
