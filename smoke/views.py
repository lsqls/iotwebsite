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
    for i in range(1, 7):
        starthour = datetime(now.year, now.month, now.day, now.hour - i, 0)
        endhour=starthour+timedelta(hours=1)
        houravg = smoke.objects.filter(uploadtime__range=[starthour,endhour ]).aggregate(Avg('value'))['value__avg']
        if(houravg):
            Hlist.append(houravg)
        else:
            Hlist.append(0)
    content={'lastest_record':lastest_record,'weekavg':weekavg,'Hlist':Hlist}
    return render(request,'smoke/index.html',content)
def upload(request,value,longitude,latitude):
    info={"value":value,'longitude':longitude,'latitude':latitude}
    if(smoke.objects.create(**info)):
        return HttpResponse("Upload success")


