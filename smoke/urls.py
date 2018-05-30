from django.conf.urls import url
from . import views
#smoke应用路径的设置
urlpatterns = [
    url(r'^$', views.index, name='index'),#PC主页，查看上传数值，曲线图以及设备位置
    url(r'^upload/(.+)-(.+)-(.+)$', views.upload, name='upload'),#该url用于上传gps数据和传感器数据
    url(r'^mindex$',views.mview,name='phone-view'),#移动端主页，显示数值
    url(r'^upload/(.+)$', views.uploadv, name='uploadv'),#该url用于上传传感器数据
]