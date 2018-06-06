# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from django.db import models
import json
# Create your models here.
#数据表的设计
class smoke(models.Model):
    uploadtime=models.DateTimeField(auto_now=True)#uploadtime 上传时间
    value=models.TextField(default='-1')#value 传感器数值
    tem = models.TextField(default='-1')  # value 传感器数值
    longitude=models.TextField(default='0')#longitude 经度数值
    latitude=models.TextField(default='0')#latitude 纬度数值
    def __unicode__(self):
       return self.value
