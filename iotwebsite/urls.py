# -*- coding: utf-8 -*-
"""iotwebsite URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/1.11/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  url(r'^$', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  url(r'^$', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.conf.urls import url, include
    2. Add a URL to urlpatterns:  url(r'^blog/', include('blog.urls'))
"""
from django.conf.urls import url,include
from django.contrib import admin
from . import views
#网站的路径设置，admin：django集成的管理界面，smoke，smoke应用界面，网站根目录是测试界面
urlpatterns = [
    url(r'^admin/', admin.site.urls),
    url(r'^smoke/',include('smoke.urls',namespace='smoke')),
    url(r'^$',views.test),
]
