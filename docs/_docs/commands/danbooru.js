// Usage:
// node danbooru.js "username" "api_key" "tagType1:tag1 tagType2:tag2" "safe" "http://source" "path/to/file.jpg"

const axios = require("axios");
const fs = require("fs");
const FormData = require("form-data");

async function createUpload(file) {
    try {
        const form = new FormData();
        form.append("upload[files][0]", fs.createReadStream(file), "test.tmp");

        const config = {
            headers: form.getHeaders(),
            maxContentLength: 999999999999,
            maxBodyLength: 999999999999,
        }
        const data = await axios.post("/uploads.json", form, config);
        return data.data.id;
    } catch (e) {
        console.error("Error creating upload: " + e.message);
        console.error(e.response.data);
        process.exit(1);
    }
}

async function getUploadMediaAssetId(id) {
    try {
        const data = await axios.get(`/uploads/${id}.json`);
        return data.data.upload_media_assets[0].id;
    } catch (e) {
        console.error("Error fetching upload information: " + e.message);
        console.error(e.response.data);
        process.exit(1);
    }
}

async function createPost(id, tags, rating, source) {
    try {
        const form = new FormData();
        form.append("upload_media_asset_id", id);
        form.append("post[tag_string]", tags);
        //form.append("post[rating]", rating.substring(0, 1).toUpperCase());
        form.append("post[source]", source);

        await axios.post("posts.json", form, { headers: form.getHeaders() });
    } catch (e) {
        console.error("Error creating post: " + e.message);
        console.error(e.response.data);
        process.exit(1);
    }
}

(async () => {
    // Get parameters
    const argv = process.argv.slice(2);
    const username = argv.shift();
    const token = argv.shift();

    // Axios settings
    axios.defaults.baseURL = "http://localhost:3000/";
    axios.defaults.headers.common["Authorization"] = "Basic " + Buffer.from(username + ":" + token).toString("base64");
    axios.defaults.headers.common["Accept"] = "application/json";

    // Create post
    const uploadId = await createUpload(argv[3]);
    const mediaAssetId = await getUploadMediaAssetId(uploadId);
    await createPost(mediaAssetId, argv[0], argv[1], argv[2]);
})();
