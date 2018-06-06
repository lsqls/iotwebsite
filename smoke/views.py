# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from .models import smoke
from django.db.models import Avg
from  datetime import datetime, timedelta
from django.shortcuts import render
from django.shortcuts import HttpResponse
from django.utils.safestring import mark_safe
from django.template import Context
# Create your views here.
#后端处理的代码
def index(request): #显示主页
    lastest_record=smoke.objects.order_by('-uploadtime')[0]#查询数据库最近上传的数据
    now = datetime.now()
    enddate = datetime(now.year, now.month, now.day, 0, 0)
    weekago = enddate - timedelta(weeks=1)
    smokeweekavg = smoke.objects.filter(uploadtime__range=[weekago, enddate]).aggregate(Avg('value'))['value__avg']
    temweekavg = smoke.objects.filter(uploadtime__range=[weekago, enddate]).aggregate(Avg('tem'))['tem__avg']
    #取数据库中迄今为止一周的数据，并求平均值
    records=smoke.objects.order_by('-uploadtime')[:10].values_list('value','tem','uploadtime')
    smokelist=[]
    temlist=[]
    uploadtimelist=[]
    for record in records:
        smokelist.append(record[0])
        temlist.append(record[1])
        uploadtimelist.append(record[2])
    smokelist = [x.encode('utf-8') for x in smokelist]
    smokelist = unicode(smokelist)
    temlist = [x.encode('utf-8') for x in temlist]
    temlist = unicode(temlist)
    uploadtimelist = [x.strftime("%H:%M:%S") for x in uploadtimelist]
    uploadtimelist=unicode(uploadtimelist)
    content={'lastest_record':lastest_record,
             'smokeweekavg':smokeweekavg,
             'temweekavg':temweekavg,
             'smokelist':smokelist,
            'temlist':temlist,
             'uploadtimelist':uploadtimelist,
             } #将数据传送给前端模板渲染
    return render(request,'smoke/index.html',content)
def upload(request,value,tem,longitude,latitude):
    #处理上传数据
    lon=float(longitude[0:3])+float(longitude[3:])/60.0#将上传的gps数据处理为标准的度表示法
    lat=float(latitude[0:2])+float(latitude[2:])/60.0
    info={"value":value,"tem":tem,'longitude':lon,'latitude':lat}#数值-经度-纬度
    if(smoke.objects.create(**info)):
        return HttpResponse("Upload success")#向客户端返回上传成功的信息
def uploadv(request,value,tem):
    #同upload功能
    info={"value":value,'tem':tem}#数值
    if(smoke.objects.create(**info)):
        return HttpResponse("Upload success")
