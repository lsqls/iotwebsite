# -*- coding: utf-8 -*-
# Generated by Django 1.11.10 on 2018-06-06 05:40
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('smoke', '0004_auto_20180409_1743'),
    ]

    operations = [
        migrations.AddField(
            model_name='smoke',
            name='tem',
            field=models.TextField(default='-1'),
        ),
    ]
