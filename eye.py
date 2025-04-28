import dlib
import cv2

# 얼굴 검출기, 랜드마크 예측기
detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor("shape_predictor_68_face_landmarks.dat")

# ✅ 웹캠 열기
cap = cv2.VideoCapture(0)

# 윈도우 설정
cv2.namedWindow('frame', cv2.WINDOW_NORMAL)
cv2.resizeWindow('frame', 800, 600)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    img_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    r = 400.0 / img_rgb.shape[1]
    dim = (400, int(img_rgb.shape[0] * r))
    resized = cv2.resize(img_rgb, dim)

    rects = detector(resized, 1)
    for rect in rects:
        l, t, r_, b = rect.left(), rect.top(), rect.right(), rect.bottom()
        shape = predictor(resized, rect)

        for j in range(68):
            x, y = shape.part(j).x, shape.part(j).y
            cv2.circle(resized, (x, y), 2, (0, 0, 255), -1)

        cv2.rectangle(resized, (l, t), (r_, b), (0, 255, 0), 2)

    # 다시 BGR로 변환해서 출력
    resized_bgr = cv2.cvtColor(resized, cv2.COLOR_RGB2BGR)
    bigger = cv2.resize(resized_bgr, (800, 600))
    cv2.imshow('frame', bigger)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# ✅ 깔끔하게 리소스 정리
cap.release()
cv2.destroyAllWindows()