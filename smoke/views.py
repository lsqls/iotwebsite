# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from .models import smoke
from django.db.models import Avg
from  datetime import datetime, timedelta
from django.shortcuts import render
from django.shortcuts import HttpResponse
# Create your views here.
def index(request):
    lastest_record=smoke.objects.order_by('-id')[0]
    now = datetime.now()
    enddate = datetime(now.year, now.month, now.day, 0, 0)
    weekago = enddate - timedelta(weeks=1)
    weekavg = smoke.objects.filter(uploadtime__range=[weekago, enddate]).aggregate(Avg('value'))['value__avg']
    Hlist = []
    for i in range(2, 8):
        starthour = datetime(now.year, now.month, now.day, now.hour - i, 0)
        endhour=starthour+timedelta(hours=1)
        houravg = smoke.objects.filter(uploadtime__range=[starthour,endhour ]).aggregate(Avg('value'))['value__avg']
        if(houravg):
            Hlist.append(houravg)
        else:
            Hlist.append(0)
    content={'lastest_record':lastest_record,'weekavg':weekavg,'Hlist':Hlist,'data0':Hlist[0],
             'data1': Hlist[1],'data2':Hlist[2],'data3':Hlist[3],'data4':Hlist[4],'data5':Hlist[5]}
    return render(request,'smoke/index.html',content)
def upload(request,value,longitude,latitude):
    lon=float(longitude[0:3])+float(longitude[3:])/60.0
    lat=float(latitude[0:2])+float(latitude[2:])/60.0
    info={"value":value,'longitude':lon,'latitude':lat}#数值-经度-纬度
    if(smoke.objects.create(**info)):
        return HttpResponse("Upload success")
def uploadv(request,value):
    info={"value":value,}#数值
    if(smoke.objects.create(**info)):
        return HttpResponse("Upload success")
def mview(request):
    lastest_record = smoke.objects.order_by('-id')[0]
    now = datetime.now()
    enddate = datetime(now.year, now.month, now.day, 0, 0)
    weekago = enddate - timedelta(weeks=1)
    weekavg = smoke.objects.filter(uploadtime__range=[weekago, enddate]).aggregate(Avg('value'))['value__avg']
    content = {'lastest_record': lastest_record, 'weekavg': weekavg}
    return render(request, 'smoke/mindex.html', content)
