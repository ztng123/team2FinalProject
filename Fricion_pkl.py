import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestRegressor
from sklearn.preprocessing import LabelEncoder
import joblib  # 모델 저장

# 1. CSV 또는 텍스트 파일을 불러왔다고 가정
df = pd.read_csv("Environment_data.csv", encoding='cp949')

# 2. 범주형 데이터 인코딩 
le_road = LabelEncoder()
le_cond = LabelEncoder()

df['도로조건'] = le_road.fit_transform(df['도로조건'])
df['환경조건'] = le_cond.fit_transform(df['환경조건'].fillna('-'))

# 3. 입력(X) / 출력(y)
X = df[['도로조건', '환경조건', '경사도(%)', '차량속도(km/h)', '차량중량(kg)']]
y = df['마찰계수(μ)']

# 4. 모델 훈련
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

model = RandomForestRegressor(n_estimators=100, random_state=42)
model.fit(X_train, y_train)

# 5. 모델 저장
joblib.dump(model, 'friction.pkl')
joblib.dump(le_road, 'road.pkl')
joblib.dump(le_cond, 'cond.pkl')

print("모델 학습 및 저장 완료!")
