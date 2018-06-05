from django.conf.urls import url
from . import views

urlpatterns = [
    # ex : /smoke/
    url(r'^$', views.index, name='index'),
    # ex : /smoke/upload/100.2-10.3-100.4
 #   url(r'^upload/(\d+.\d+)-(\d+.\d+)-(\d+.\d+)$', views.upload, name='upload'),
    url(r'^upload/(.+)-(.+)-(.+)-(.+)$', views.upload, name='upload'),
    url(r'^mindex$',views.mview,name='phone-view'),
    url(r'^upload/(.+)$', views.uploadv, name='uploadv'),
]