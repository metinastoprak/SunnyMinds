#!/bin/bash

# read versiyon name
read -p "PQ versiyon bilgisini giriniz" version

# Versiyon bilgisi kontrolü
if [[ -z "$version" ]]; then
    echo "Hata: Bilgi girilmedi"
    exit 1
fi

# check PQ.b ve PQ_COMMON.b files whether exist or not
if [[ ! -f "PQ.b" || ! -f "PQ_COMMON.b" ]]; then
    echo "Hata--> PQ.b ve PQ_COMMON.b dosyalar mevcut değil."
    exit 1
fi

# Check DB.ini file
if [[ ! -f "DB.ini" ]]; then
    echo "Hata--> DB.ini dosyası mevcut değil."
    exit 1
fi

# get configversion from DB.ini
configure_version=$(grep "ConfigureVersion" DB.ini | awk -F'=' '{print $2}' | tr -d '[:space:]')
if [[ -z "$configure_version" ]]; then
    echo "Hata-->geçerli versiyon değeri bulunamadı."
    exit 1
fi

version_suffix=$(echo "$configure_version" | cut -d'_' -f2 | cut -c1-4)
if [[ -z "$version_suffix" ]]; then
    echo "Hata--> digitler alınamadı."
    exit 1
fi

# create ZIP files
pq_zip_name="${version}.zip"
db_zip_name="db_${version_suffix}.zip"

# add files 
zip -j "$pq_zip_name" PQ.b PQ_COMMON.b
if [[ $? -ne 0 ]]; then
    echo "Hata--> $pq_zip_name dosyası oluşturulamadı."
    exit 1
fi

# add DB.ini into ZIP file
zip -j "$db_zip_name" DB.ini
if [[ $? -ne 0 ]]; then
    echo "Hata: $db_zip_name dosyası oluşturulamadı."
    exit 1
fi

echo " dosyalar tamamlandı"
echo "  - $pq_zip_name (PQ.b  PQ_COMMON.b)"
echo "  - $db_zip_name (DB.ini)"
